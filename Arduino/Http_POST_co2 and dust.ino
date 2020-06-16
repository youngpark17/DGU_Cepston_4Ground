#include <WiFi.h>
#include <HTTPClient.h>
#include <HttpClient.h>

//#include <Wire.h>

 
const char* ssid = "SW";
const char* password =  "csid2018!";
const char* serverName = "http://54.167.55.244:5000/";
int value_co2;
int value_dust;
int sensor_number = 1;
int analog = 34;

IPAddress hostIp(54, 167, 55, 244);
int SERVER_PORT = 5000;
WiFiClient client;

void setup() {
 
  Serial.begin(115200);
  pinMode(25, INPUT);


  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
 
}
 
void loop() {
  
  float value_dust = analogRead(25);
  value_dust= value_dust*(22.0 / 1023.0);
  Serial.print("Dust : ");
  Serial.println(value_dust);

 
 if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status

     for(int i=0; i<8; i++){
      value_co2+=analogRead(analog);
      delay(200);
     }
     value_co2/=8;
     int adcVal = value_co2;
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
    float value_co2 =(voltageDiference*5000.0)/1.6;
    Serial.print("voltageDiference : ");
    Serial.println(voltageDiference);
  
    Serial.print("value_co2 :");
    Serial.println(value_co2);

    }
     HTTPClient http;
   
     http.begin("http://54.167.55.244:5000/");  //Specify destination for HTTP request
     http.addHeader("Content-Type",  "application/x-www-form-urlencoded");             //Specify content-type header
  
     String httpRequestData = "sensor_number_co2="+String(sensor_number)+"&sensor_number_dust="+String(sensor_number)+"&value_co2="+String(value_co2)+"&value_dust="+String(value_dust);
     int httpResponseCode = http.POST(httpRequestData);   //Send the actual POST request
   
     if(httpResponseCode>0){
  
      String response = http.getString();                       //Get the response to the request
   
      Serial.println(httpResponseCode);   //Print return code
      Serial.println(response);           //Print request answer
      Serial.println("");
   
     }else{
   
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
   
     }
   
     http.end();  //Free resources
   
 }else{

    Serial.println("Error in WiFi connection");   
 }
 
    delay(2000);  //Send a request every 10 seconds
    //10minute = 600seconds = 600000

}
