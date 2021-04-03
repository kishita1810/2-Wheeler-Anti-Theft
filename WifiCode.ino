#include "ESP8266WiFi.h"
#include "WiFiClient.h"
#include "ESP8266WebServer.h" 
#include "ESP8266mDNS.h"


//for wifi module
ESP8266WebServer server(80); //port http 

const char ssid     = "2 wheeler anti-theft"; // Enter your Wi-Fi name 
const char password = "2WATD" ; // Enter your Wi-Fi password


int led = 4;

void setup()
{
  Serial.begin(9600);
  
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);
  WiFi.begin(ssid, password); 
  Serial.println(""); 

   // Wait for connection 
   while (WiFi.status() != WL_CONNECTED) { 
   delay(500); 
   Serial.print("."); 
   } 
   Serial.println(""); 
   Serial.print("Connected to "); 
   Serial.println(ssid); 
   Serial.print("IP address: ");
   if (MDNS.begin("esp8266")) { 
   Serial.println("MDNS responder started"); 
   } 
   server.on("/", handleRoot); 
   server.on("/inline", [](){ 
   server.send(200, "text/plain", "this works as well");  }); 
   server.onNotFound(handleNotFound); 
   server.begin(); 
   Serial.println("HTTP server started"); 

}


void loop()
{
  digitalWrite(led,HIGH);
  server.handleClient(); 
}

void handleRoot() { 
  
  server.send(200, "text/plain"); 
} 

void handleNotFound(){ 

   String message = "File Not Found\n\n"; 
   message += "URI: "; 
   message += server.uri(); 
   message += "\nMethod: "; 
   message += (server.method() == HTTP_GET)?"GET":"POST"; 
   message += "\nArguments: "; 
   message += server.args(); 
   message += "\n"; 
   for (uint8_t i=0; i<server.args(); i++){ 
   message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; 
   } 
   server.send(404, "text/plain", message);
  
} 
