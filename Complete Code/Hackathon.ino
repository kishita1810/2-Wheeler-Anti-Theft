// Include Libraries
#include "Arduino.h"
#include "ADXL345.h"
#include "I2Cdev.h"
#include "ESP8266.h"
#include "NewPing.h"
#include "Adafruit_NeoPixel.h"
#include "Relay.h"
#include "TinyGPS++.h"
#include "SoftwareSerial.h"
#include "ESP8266WiFi.h"
#include "WiFiClient.h"
#include "ESP8266WebServer.h" 
#include "ESP8266mDNS.h"



//for wifi module
ESP8266WebServer server(80); //port http 
// ====================================================================
// vvvvvvvvvvvvvvvvvvvv ENTER YOUR WI-FI SETTINGS  vvvvvvvvvvvvvvvvvvvv
//
const char ssid     = "2 wheeler anti-theft"; // Enter your Wi-Fi name 
const char password = "2WATD" ; // Enter your Wi-Fi password
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ====================================================================



//for ultrasonic
int trigPin = 3;
int echoPin = 2;
int led = 4;
int us=0;

//for accelerometer
int Xread;
int Xrest;
 
int Yread;
int Yrest;
 
int Zread;
int Zrest;
 
double Gx;
double Gy;
double Gz;
 
int xpin = 0;
int ypin = 1;
int zpin = 2;
 
int t1;
int acc=0;

//for gps and gsm module
static const int RXPin = 11, TXPin = 10;
static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;
int temp=0,i;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
String stringVal = "";

//for relay
int relay = 5;

void setup()
{
  Serial.begin(9600);
  
  //for wifi
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



  //for ultrasonic
   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);

   //for accelerometer  
    
    Xrest=analogRead(xpin);
    Serial.print(Xrest);
    Yrest=analogRead(ypin);
    Serial.print(Yrest);
    Zrest=analogRead(zpin);
    Serial.print(Zrest);
    digitalWrite(13,LOW);

    //for gps and gsm module
    ss.begin(GPSBaud);
    pinMode(13,OUTPUT);
    digitalWrite(13,LOW);
    gsm_init(); 
    Serial.println("AT+CNMI=2,2,0,0,0");
    temp=0;

    //for relay
    pinMode(7, OUTPUT);// connected to S terminal of Relay
}

void loop()
{
  void wifi();
  void gpsandgsm();  
}

void wifi()
{
   relay();
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

   

void relay()
{

  if(ssid == "2 wheeler anti-theft" && password == "2WATD") 
    digitalWrite(relay,HIGH);// turn relay ON
    
    delay(15000);
    digitalWrite(relay, LOW);// turn relay OFF
  else()
  {
    digitalWrite(relay, LOW);// turn relay OFF
  }
    
}

void ultrasonic()
{
  long duration, distance;
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);
  duration=pulseIn(echoPin, HIGH);
  distance =(duration/2)/29.1;
  Serial.print(distance);
  Serial.println("CM");
  delay(10);
 
  if((distance<=10)) 
    {
      digitalWrite(led, LOW);
      us = 0;
    }
  else if(distance>10)
    {
     digitalWrite(led, HIGH);
     us = 1;
    }
}

void acceleromter()
{
  Serial.print("Time ");
  t1=millis();
  Serial.println(t1*0.001);
   
  Xread = analogRead(xpin)-Xrest;
  Yread=analogRead(ypin)-Yrest;
  Zread=analogRead(zpin)-Zrest;
   
  Gx=Xread/67.584;
  Gy=Yread/67.584;
  Gz=Zread/67.584;
   
  Serial.print("Acceleration X :");
  Serial.print(Gx);
  Serial.print("Acceleration Y :");
  Serial.print(Gy);
  Serial.print("Acceleration Z :");
  Serial.print(Gz);
  Serial.print("\n");

  if((Gz<=10)) 
    {
      acc = 0;
    }
  else if(Gz>10)
    {
     acc = 1;
    }
}

void gpsandgsm()
{
  ultrasonic();
  acceleromter();

  if(us==1 || acc==1)
      while(1)
      {
        while (ss.available() > 0)
        {
          gps.encode(ss.read());
                if (gps.location.isUpdated())
                {
                 us=0;
                 acc=0;
                 digitalWrite(13,HIGH);
                 tracking();
                } 
          if( !us || !acc )
          break;  
        }
      }
      digitalWrite(13,LOW);

}


void gsm_init()
{
  
  boolean at_flag=1;
  while(at_flag)
  {
    Serial.println("AT");
    delay(1);
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      at_flag=0;
    }
    
    delay(1000);
  }
  boolean echo_flag=1;
  while(echo_flag)
  {
    Serial.println("ATE0");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      echo_flag=0;
    }
    delay(1000);
  }

  boolean net_flag=1;
  while(net_flag)
  {
    Serial.println("AT+CPIN?");
    while(Serial.available()>0)
    {
      if(Serial.find("+CPIN: READY"))
      net_flag=0;
    }
    delay(1000);
  }
}
void init_sms()
{
  Serial.println("AT+CMGF=1");
  delay(400);
  Serial.println("AT+CMGS=\"8999965409\"");   // use your 10 digit cell no. here
  delay(400);
}
void send_data(String message)
{
  Serial.print(message);
  delay(200);
}
void send_sms()
{
  Serial.write(26);
}

void tracking()
{
    init_sms();
    send_data("Vehicle Tracking Alert:");
    Serial.println(" "); 
    send_data("Your Vehicle Current Location is:");
    Serial.println(" "); 
    Serial.print("Latitude: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print("\n Longitude: ");
    Serial.println(gps.location.lng(), 6);

   // https://www.google.com/maps/@8.2630696,77.3022699,14z
   Serial.print("https://www.google.com/maps/@");
   Serial.print(gps.location.lat(), 6);
   Serial.print(',');
   Serial.print(gps.location.lng(), 6);
   Serial.print(",14z");
   send_sms();
   delay(2000);
}
