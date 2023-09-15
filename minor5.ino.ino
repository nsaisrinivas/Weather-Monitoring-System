#include <ESP8266WiFi.h>
#include "DHT.h"
#include <SFE_BMP180.h>
#include <Wire.h>

#include<ThingSpeak.h>
 
#define ALTITUDE 431.0
#define DHTPIN 0     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
 
SFE_BMP180 pressure;
DHT dht(DHTPIN, DHTTYPE);
const char* ssid     = "saisrinivas"; // Your ssid
const char* password = "Gayathris"; // Your Password

WiFiClient client;  
 unsigned long myChannelNumber = 1400536;  
 const char * myWriteAPIKey = "9FAQFX9YPJPXKFTG";
 
int sensorPin = A0;    // input for LDR and rain sensor
int enable2 = 13;      // enable reading Rain sensor
int sensorValue2 = 0;


char status;
double T,P,p0,a;
String result;
WiFiServer server(80);
void setup() {
Serial.begin(115200);
delay(100);
dht.begin();
pinMode(enable2, OUTPUT);
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
 
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
 
Serial.println("");
Serial.println("WiFi is connected");
server.begin();
Serial.println("Server started");
 
 
Serial.println(WiFi.localIP());
ThingSpeak.begin(client);
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
  delay(500);
}

void loop() {
    status = pressure.getPressure(P,T);//get the pressure and stores the value in P
    if (status != 0)
    {
      Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
p0 = pressure.sealevel(P,ALTITUDE); // we're at 431 meters (Boulder, CO)
Serial.print("provided altitude:");
Serial.print(ALTITUDE);
Serial.print("relative (sea-level) pressure: ");
Serial.print(p0,2);
Serial.print(" mb, ");
Serial.println("");
}
ThingSpeak.writeField(myChannelNumber, 3, (float)p0, myWriteAPIKey);
delay(1000);
sensorValue2 = analogRead(sensorPin);
sensorValue2 = constrain(sensorValue2, 150, 440); //limits the value of sensorvalue2 in between 150 to 440 constrain(variable,low,high);
sensorValue2 = map(sensorValue2, 150, 440, 1023, 0); //used to scale the two different ranges map(variable,from low,from high,to low,to high);
if (sensorValue2>= 20)
{
  result="rain is detected";
  Serial.print(result);
  
digitalWrite(enable2, HIGH);
  }
  else
  
{
  result= "rain not detected";
  Serial.print(result);
  digitalWrite(enable2, LOW); 
  }
Serial.print(",");
Serial.print("Rain value:");
Serial.println(sensorValue2);
delay(100);
ThingSpeak.writeField(myChannelNumber, 4, (String)sensorValue2, myWriteAPIKey);
float h = dht.readHumidity();
Serial.print("Humidity:");
Serial.print(h);
Serial.print(",");
delay(2000);
// Read temperature as Celsius (the default)
float t = dht.readTemperature();
Serial.print("Temperature:");
Serial.print(t);
Serial.print("°c,");
// Read temperature as Fahrenheit (isFahrenheit = true)
float f = dht.readTemperature(true);
Serial.print(f);
Serial.print("°f");
Serial.println("");
delay(1000);
ThingSpeak.writeField(myChannelNumber, 1, t, myWriteAPIKey);
delay(2000);
ThingSpeak.writeField(myChannelNumber, 2, h, myWriteAPIKey);
delay(1000);
WiFiClient client = server.available();
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println("Connection: close");  // the connection will be closed after completion of the response
client.println("Refresh: 10");  // update the page after 10 sec
client.println();
client.println("<!DOCTYPE HTML>");
client.println("<html>");
client.println("<style>html { font-family:'Times New Roman',serif; display: block; margin: 0px auto; text-align: center;color: #333333; background-color: lightgray;}");
client.println("body{margin-top: 50px;}");
client.println("h1 {font-family:'Times New Roman', sans-serif; margin: 50px auto 30px; font-size: 50px; text-align: center;color:black}");
client.println(".side_adjust{display: inline-block;vertical-align: middle;position: relative;}");
client.println(".text1{font-weight: 180; padding-left: 15px; font-size: 50px; width: 170px; text-align: left; color: #3498db;}");
client.println(".data1{font-weight: 180; padding-left: 80px; font-size: 50px;color: #3498db;}");
client.println(".text2{font-weight: 180; font-size: 50px; width: 170px; text-align: left; color: Tomato;}");
client.println(".data2{font-weight: 180; padding-left: 150px; font-size: 50px;color: Tomato;}");
client.println(".text3{font-weight: 180; padding-left: 15px; font-size: 50px; width: 170px; text-align: left; color: #0066ff;}");
client.println(".data3{font-weight: 180; padding-left: 80px; font-size: 50px;color: #0066ff;}");
client.println(".data4{font-weight: 180; padding-left: 10px; font-size: 50px;color: #0066ff;}");
client.println(".result{font-family:'Times New Roman', sans-serif; margin: 50px auto 30px; font-size: 50px; text-align: center;color:Tomato;}");
client.println(".data{padding: 10px;}");
client.println("</style>");
client.println("</head>");
client.println("<body>");
client.println("<div id=\"webpage\">");   
client.println("<marquee direction=\"right\" scrollamount=\"12\"><h1>!!!Weather Station server!!!</h1></marquee>");
client.println("<div class=\"data\">");
client.println("<div class=\"side_adjust text1\">Humidity:</div>");
client.println("<div class=\"side_adjust data1\">");
client.print(h);
client.println("<div class=\"side_adjust text1\">%</div>");
client.println("</div>");
client.println("<div class=\"data\">");
client.println("<div class=\"side_adjust text2\">Temperature:</div>");
client.println("<div class=\"side_adjust data2\">");
client.print(t);
client.println("<div class=\"side_adjust text2\">*C</div>");
client.print(f);
client.println("<div class=\"side_adjust text2\">*F</div>");
client.println("</div>");
client.println("<div class=\"data\">");
client.println("<div class=\"side_adjust text3\">Pressure:</div>");
client.println("<div class=\"side_adjust data3\">");
client.print(p0,2);
client.println("<div class=\"side_adjust text3\">mb</div>");
client.println("</div>");
client.println("<div class=\"data\">");
client.println("<div class=\"side_adjust text3\">Rain:</div>");
client.println("<div class=\"side_adjust data4\">");
client.print(sensorValue2);
client.println("</div>");
client.println("<div class=\"data\">");
client.println("<div class=\"result\">");
client.print(result);
client.println("</div>");
client.println("</div>");
client.println("</body>");
client.println("</html>");
 delay(4000);
}
