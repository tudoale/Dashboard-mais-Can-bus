/*By Alessandro Ferreira 24/08/2023 V1.0*/
/*
  -------------------------
  SERVIDOR AP - Dynamic Example
  -------------------------
  LEITURA CAN BUS
  -------------------------
  EXIBE NUMA PAGINA HTML DADOS DA CAN
  -------------------------

  Works with both ESP32

  -------------------------------

*/

#define CAN_2515
// #define CAN_2518FD

#define SERIAL_PORT_MONITOR Serial

// Set SPI CS Pin according to your hardware

#if defined(SEEED_WIO_TERMINAL) && defined(CAN_2518FD)

const int SPI_CS_PIN  = BCM8;
const int CAN_INT_PIN = BCM25;
#else

const int SPI_CS_PIN = 5;
const int CAN_INT_PIN = 2;
#endif

#ifdef CAN_2515
#include "mcp2515_can.h"
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
#endif
// Load Wi-Fi library


int V1, V2, V3, V4, V5, V6, V7, V8 = 0; //Variaveis Globais que recebem valor da CAN
int var;
int CH;
/*===================================================================*/
/*-------------------------------------------------------------------*/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


// Replace with your network credentials
const char* ssid     = "AIZ_TECH";
const char* password = "123456789";

/* Your WiFi Credentials */
//const char* ssid = "TP-Link_8F7F"; // SSID
//const char* password = "66152613"; // Password

/* Start Webserver */
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 300;

int temperature, temper;
int rpm;
int pressure;
int Acceleration;
int Throttle;
int brake;

void pgn(int var)
{
  switch (var) 
  {
    case 39: Serial.println("Entrou no 39");  break;
    case 2:  Serial.println("Entrou no 2");  break;
    default:
    // Se nenhum dos anteriores, faz o caso padrão default
    // default é opcional
    break;
  }
}


void getSensorReadings(){
  temperature = random(0,40);   
  rpm = random(0, 100);
  pressure = random(0,100);
  Acceleration = random(0,40);   
  Throttle = random(0, 100);
  brake = random(0,100);
}

String processor(const String& var){
  getSensorReadings();
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(temperature);
  }
  else if(var == "RPM"){
    return String(rpm);
  }
  else if(var == "PRESSURE"){
    return String(pressure);
  }
  else if(var == "ACCELERATION"){
    return String(Acceleration);
  }
  else if(var == "THROTTLE"){
    return String(Throttle);
  }
  else if(var == "BRAKE"){
    return String(brake);
  }
  
  return String();
}


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>ESP Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <link rel="icon" href="data:,">
    <style>
        html {
            font-family: Arial;
            display: inline-block;
            text-align: center;
        }

        p {
            font-size: 1.2rem;
        }

        body {
            margin: 0;
        }

        .topnav {
            overflow: hidden;
            background-color: #50B8B4;
            color: white;
            font-size: 1rem;
        }

        .content {
            padding: 20px;
        }

        .card {
            background-color: white;
            box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
        }

        .cards {
            max-width: 800px;
            margin: 0 auto;
            display: grid;
            grid-gap: 2rem;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
        }

        .reading {
            font-size: 1.4rem;
        }
    </style>
</head>
<body>
    <div class="topnav">
        <h1>CAN BUS WEB SERVER</h1>
    </div>
    <div class="content">
        <div class="cards">
            <div class="card">
                <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> TEMPERATURE</p><p><span class="reading"><span id="temp">%TEMPERATURE%</span> &deg;C</span></p>
            </div>
            <div class="card">
                <p><i class="fas fa-tint" style="color:#00add6;"></i> RPM</p><p><span class="reading"><span id="rp">%RPM%</span> &percnt;</span></p>
            </div>
            <div class="card">
                <p><i class="fas fa-angle-double-down" style="color:#e1e437;"></i> PRESSURE</p><p><span class="reading"><span id="pres">%PRESSURE%</span> hPa</span></p>
            </div>
            <div class="card">
                <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> Acceleration Sensor</p><p><span class="reading"><span id="Accel">%ACCELERATION%</span> &percnt;C</span></p>
            </div>
            <div class="card">
                <p><i class="fas fa-tint" style="color:#00add6;"></i> Engine Throttle</p><p><span class="reading"><span id="Throt">%THROTTLE%</span> &percnt;</span></p>
            </div>
            <div class="card">
                <p><i class="fas fa-angle-double-down" style="color:#e1e437;"></i> Electronic Brake</p><p><span class="reading"><span id="brak">%BRAKE%</span> Pa</span></p>
            </div>
        </div>
    </div>
    <script>
if (!!window.EventSource) {
 var source = new EventSource('/events');

 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);

 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);

 // Dados a serem alterados 
 source.addEventListener('temperature', function(e) {
  console.log("temperature", e.data);
  document.getElementById("temp").innerHTML = e.data;
 }, false);

 source.addEventListener('rpm', function(e) {
  console.log("rpm", e.data);
  document.getElementById("rp").innerHTML = e.data;
 }, false);

 source.addEventListener('pressure', function(e) {
  console.log("pressure", e.data);
  document.getElementById("pres").innerHTML = e.data;
 }, false);

 // Dados a serem alterados 
    source.addEventListener('Acceleration', function (e) {
        console.log("Acceleration", e.data);
  document.getElementById("Accel").innerHTML = e.data;
 }, false);

    source.addEventListener('Throttle', function (e) {
        console.log("Throttle", e.data);
        document.getElementById("Throt").innerHTML = e.data;
 }, false);

     source.addEventListener('brake', function (e) {
     console.log("brake", e.data);
     document.getElementById("brak").innerHTML = e.data;
 }, false);
}
    </script>
</body>
</html>)rawliteral";


void setup() 
{
  Serial.begin(115200);

  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

   // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 300);
  });
  server.addHandler(&events);
  
  server.begin();  
  
  /*__________________________________________________________________________________________________*/
  /*__________________________________________________________________________________________________*/
  
  while (CAN_OK != CAN.begin(CAN_250KBPS, MCP_8MHz)) 
  {             // init can bus : baudrate = 250k, Clock MCP_2515 = 8MHz
        SERIAL_PORT_MONITOR.println("CAN init fail, retry...");        
        delay(100);
  }
    SERIAL_PORT_MONITOR.println("CAN init ok!");
}

// INICIO LOOP
void loop() 
{
  // FAZ A LEITURA DA CAN
    unsigned char len = 0;
    unsigned char buf[8];

      if (CAN_MSGAVAIL == CAN.checkReceive()) 
      {         // check if data coming
          CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

          unsigned long canId = CAN.getCanId();
          SERIAL_PORT_MONITOR.println(canId, HEX);
          
          for (int i = 0; i < len; i++) 
          { // print the data
              if(i == 0) V1 = 10 * buf[i];
              if(i == 1) V2 = buf[i]; 
              if(i == 2) V3 = buf[i];
              if(i == 3) V4 = buf[i];
              if(i == 4) V5 = buf[i];
              if(i == 5) V6 = buf[i]; 
              if(i == 6) V7 = buf[i]; 
              if(i == 7) V8 = buf[i];    
            
          }
      }
    /*__________________________________________________________________________________________________*/
  
    /*__________________________________________________________________________________________________*/
    //_____Envia os DADOS para exibir na pagina html
      if ((millis() - lastTime) > timerDelay) 
      {
          getSensorReadings();
          //Serial.printf("Temperature = %.2f ºC \n", temperature);
          pgn(temperature);
          //Serial.println(CH);

          // Send Events to the Web Client with the Sensor Readings
          events.send("ping",NULL,millis());
          events.send(String(temperature).c_str(),"temperature",millis());
          events.send(String(rpm).c_str(),"rpm",millis());
          events.send(String(pressure).c_str(),"pressure",millis());
          events.send(String(Acceleration).c_str(),"Acceleration",millis());
          events.send(String(Throttle).c_str(),"Throttle",millis());
          events.send(String(brake).c_str(),"brake",millis());
        
          lastTime = millis();
      }
    //delay(100);
}
