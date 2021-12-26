#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <Wire.h>              
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

Servo testservo;

int laserPin = 18;
#define LDR 33
#define trig 5
#define echo 4 
  
float latitude , longitude;
String  lat_str , lng_str;
const unsigned long petainterval = 0.2;
const unsigned long permeninterval = 0.1;
const char *ssid =  "WR";     // Enter your WiFi Name
const char *pass =  "cerberus26"; // Enter your WiFi Password
unsigned long permentimer;
unsigned long petatimer;
char auth[] = "HkwMslyhbsEJOhbHtnZjfye6FcJFu_MP"; 
WiFiClient client;
TinyGPSPlus gps;
HardwareSerial SerialGPS(1);

void setup()
{
  Serial.begin(115200);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  permentimer = millis ();
  petatimer = millis ();
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");              // print ... till not connected
  }
  Serial.println("");
  Serial.println("WiFi connected");
  pinMode(laserPin, OUTPUT);
  pinMode (trig, OUTPUT);
  pinMode (echo, INPUT);
  testservo.attach(13);
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V0, "clr"); 
}

void peta(){
  digitalWrite(trig, LOW);
  delay(10);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
   
  long durasi;
  float jarak;
   
  durasi = pulseIn (echo, HIGH);
  jarak = durasi * 0.034/2;

  Serial.print("Jarak (cm) : ");
  Serial.print(jarak);
  Serial.println(" cm");
  
  if (jarak < 20 ){
  
    while (SerialGPS.available() > 0) {
      if (gps.encode(SerialGPS.read()))
      {
        if (gps.location.isValid())
        {
          latitude = gps.location.lat();
          lat_str = String(latitude , 6);
          longitude = gps.location.lng();
          lng_str = String(longitude , 6);
          Serial.print("Latitude = ");
          Serial.println(lat_str);
          Serial.print("Longitude = ");
          Serial.println(lng_str);
          Blynk.virtualWrite(V0, 1, latitude, longitude, "Location");
        }
      }
    }
  }
   else{
     Blynk.virtualWrite(V0, "clr"); 
   }
  Blynk.run(); 
  petatimer = millis ();   
}

void permen(){
  int val = analogRead(LDR);
  Serial.println(val);
  if (val >= 1000){
     testservo.write(0);
  }
  else{
      testservo.write(120);
  } 
  permentimer = millis ();  
}

void loop()
{
  digitalWrite(laserPin, HIGH);
  if ( (millis () - petatimer) >= petainterval)
    peta();

  // The other LED is controlled the same way. Repeat for more LEDs
  if ( (millis () - permentimer) >= permeninterval) 
    permen();
  
}
