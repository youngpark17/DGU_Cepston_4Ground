#include <WiFi.h>
#include <HTTPClient.h>
#include <HttpClient.h>

//#include <Wire.h>

 
const char* ssid = "TWOSOME";
const char* password =  "twosome2644";
const char* serverName = "http://54.167.55.244:5000/";
int value;
int sensor_number = 1;
int analog = 34;

IPAddress hostIp(54, 167, 55, 244);
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

     for(int i=0; i<8; i++){
      value+=analogRead(analog);
      delay(200);
     }
     value/=8;
     int adcVal = value;
     float voltage = adcVal*(3.3/4095.0);
    if(voltage == 0)
    {
    Serial.println("A problem has occurred with the sensor.");
    }
    else if(voltage < 0.4)
    {
    Serial.println("Pre-heating the sensor...");
    }
    else
    {
 
    float voltageDiference=voltage-0.4;
    float concentration=(voltageDiference*5000.0)/1.6;
    Serial.println(voltageDiference);
    Serial.println("voltageDiference");
    Serial.println(concentration);
    Serial.println("concentration");
    
    }
     HTTPClient http;
   
     http.begin("http://54.167.55.244:5000/");  //Specify destination for HTTP request
     http.addHeader("Content-Type",  "application/x-www-form-urlencoded");             //Specify content-type header
  
     String httpRequestData = "sensor_number="+String(sensor_number)+"&value="+String(value);
     int httpResponseCode = http.POST(httpRequestData);   //Send the actual POST request
   
     if(httpResponseCode>0){
  
      String response = http.getString();                       //Get the response to the request
   
      //Serial.println(httpResponseCode);   //Print return code
      //Serial.println(response);           //Print request answer
   
     }else{
   
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
   
     }
   
     http.end();  //Free resources
   
 }else{

    Serial.println("Error in WiFi connection");   
 }
 
  delay(500);  //Send a request every 10 seconds
 
}
