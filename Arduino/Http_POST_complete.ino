#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <HttpClient.h>
//#include <Wire.h>

 
const char* ssid = "SW";
const char* password =  "csid2018!";
const char* serverName = "http://54.236.26.84:5000/";
int value;
int sensor_number = 12;
int analog = A0;

IPAddress hostIp(54, 236, 26, 84);
int SERVER_PORT = 5000;
WiFiClient client;

void setup() {
 
  Serial.begin(115200);

  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
 
}
 
void loop() {
 
 if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
/* 
  if(client.connect(hostIp, SERVER_PORT)) {
 
    String jsondata = "";
  
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["value"] = value;
    root["sensor_number"] = sensor_number;
  
    root.printTo(jsondata);
    Serial.println(jsondata);
  
    client.print(F("POST /test"));
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Cache-Control: no-cache\r\n"));
    client.print(F("Host: 54.236.26.84:5000\r\n"));
    client.print(F("User-Agent: Arduino\r\n"));
    client.print(F("Content-Type: application/json\r\n"));
    client.print(F("Content-Length: "));
    client.println(jsondata.length());
    client.println();
    client.println(jsondata);
    client.print(F("\r\n\r\n")); 

    client.println("POST /API/putData HTTP/1.1");
    client.println("Cache-Control: no-cache");
    client.println("Host: 54.236.26.84");
    client.println("Content-Type: application/json; charset=UTF-8");
    client.println("Content-Length: ");
    client.println(jsondata.length());
    client.println();
    client.println(jsondata);
    } else {
    Serial.println("Connection Failed");
  } */
     
     value = analogRead(analog);
     HTTPClient http;   
   
     http.begin("http://54.167.55.244:5000/");  //Specify destination for HTTP request
     http.addHeader("Content-Type",  "application/x-www-form-urlencoded");             //Specify content-type header
  
     String httpRequestData = "sensor_number="+String(sensor_number)+"&value="+String(value);
     Serial.println(httpRequestData);
     int httpResponseCode = http.POST(httpRequestData);   //Send the actual POST request
   
     if(httpResponseCode>0){
  
      String response = http.getString();                       //Get the response to the request
   
      Serial.println(httpResponseCode);   //Print return code
      Serial.println(response);           //Print request answer
   
     }else{
   
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
   
     }
   
     http.end();  //Free resources
   
 }else{

    Serial.println("Error in WiFi connection");   
 }
 
  delay(1000);  //Send a request every 10 seconds
 
}
