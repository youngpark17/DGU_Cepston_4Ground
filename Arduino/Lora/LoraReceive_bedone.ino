#include <WiFi.h>
#include <HTTPClient.h>
#include <HttpClient.h>
//#include "ESPAsyncWebServer.h"

#include <SPIFFS.h>

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 433E6

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Replace with your network credentials
const char* ssid     = "SW";
const char* password = "csid2018!";
const char* serverName = "http://54.167.55.244:5000/";

// Define NTP Client to get time
//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP);

// Initialize variables to get and save LoRa data
int rssi;
String loRaMessage;
String CO2;
String Dust;
String readingID;
int sensor_number = 1;

IPAddress hostIp(54, 167, 55, 244);
int SERVER_PORT = 5000;
WiFiClient client;

// Create AsyncWebServer object on port 80
//AsyncWebServer server(80);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);


//Initialize OLED display
void startOLED(){
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA SENDER");
}

//Initialize LoRa module
void startLoRA(){
  int counter;
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    Serial.println("Starting LoRa failed!"); 
  }
  Serial.println("LoRa Initialization OK!");
  display.setCursor(0,10);
  display.clearDisplay();
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);
}

void connectWiFi(){
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("connecting to WiFi...");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(serverName);
  display.setCursor(0,20);
  display.print("Access web server at: ");
  display.setCursor(0,30);
  display.print(WiFi.localIP());
  display.display();
}

// Read LoRa packet and get the sensor readings
void getLoRaData() {
  Serial.print("Lora packet received: ");
  // Read packet
  while (LoRa.available()) {
    String LoRaData = LoRa.readString();
    // LoRaData format: readingID/temperature&soilMoisture#batterylevel
    // String example: 1/27.43&654#95.34
    Serial.print(LoRaData); 
    
    // Get readingID, temperature and soil moisture
    int pos1 = LoRaData.indexOf('/');
    int pos2 = LoRaData.indexOf('&');
   // int pos3 = LoRaData.indexOf('#');
    readingID = LoRaData.substring(0, pos1);
    CO2 = LoRaData.substring(pos1 +1, pos2);
    Dust = LoRaData.substring(pos2+1, LoRaData.length());
   // pressure = LoRaData.substring(pos3+1, LoRaData.length());    
   display.setCursor(0,40);
   display.print(CO2);
   display.setCursor(0,50);
   display.print(Dust);
   display.display();
   Serial.println(CO2);
   Serial.println(Dust);
  }
  // Get RSSI
  rssi = LoRa.packetRssi();
  Serial.print(" with RSSI ");    
  Serial.println(rssi);
}


void setup() { 
  // Initialize Serial Monitor
  Serial.begin(115200);
  startOLED();
  startLoRA();
  connectWiFi();
  
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Start server
  //server.begin();
  
}

void loop() {
  // Check if there are LoRa packets available
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    getLoRaData();
    //getTimeStamp();
  }

  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status

//     for(int i=0; i<8; i++){
//      value_co2+=analogRead(analog);
//      delay(200);
//     }
//     value_co2/=8;
//     int adcVal = value_co2;
//     float voltage = adcVal*(3.3/4095.0);
//    if(voltage == 0)
//    {
//    Serial.println("A problem has occurred with the sensor.");
//    }
//    else if(voltage < 0.4)
//    {
//    Serial.println("Pre-heating the sensor...");
//    }
//    else
//    {
// 
//    float voltageDiference=voltage-0.4;
//    float value_co2 =(voltageDiference*5000.0)/1.6;
//    Serial.print("voltageDiference : ");
//    Serial.println(voltageDiference);
//  
//    Serial.print("value_co2 :");
//    Serial.println(value_co2);
//
//    }
     HTTPClient http;
   
     http.begin("http://54.167.55.244:5000/");  //Specify destination for HTTP request
     http.addHeader("Content-Type",  "application/x-www-form-urlencoded");             //Specify content-type header
  
     String httpRequestData = "sensor_number_co2="+String(sensor_number)+"&sensor_number_dust="+String(sensor_number)+"&value_co2="+String(CO2)+"&value_dust="+String(Dust);
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
 
    delay(1000);  //Send a request every 10 seconds
    //10minute = 600seconds = 600000

  
}
