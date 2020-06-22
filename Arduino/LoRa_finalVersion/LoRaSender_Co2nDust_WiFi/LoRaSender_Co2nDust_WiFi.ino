#include <SPI.h>
#include <LoRa.h>
#include <stdlib.h>

//WiFi connection
#include <WiFi.h>
#include <HTTPClient.h>
#include <HttpClient.h>


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

//BME280 definition
#define SDA 21
#define SCL 13

TwoWire I2Cone = TwoWire(1);
//Adafruit_BME280 bme;

//WiFi
const char* ssid = "SW";
const char* password =  "csid2018!";
const char* serverName = "http://54.167.55.244:5000/";

IPAddress hostIp(54, 167, 55, 244);
int SERVER_PORT = 5000;
WiFiClient client;

//packet counter
int readingID = 0;

int counter = 0;
int value_co2;
float value_dust;
int sensor_number = 1;
int analog = 34;
String LoRaMessage = "";

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
    readingID++;
    Serial.println("Starting LoRa failed!"); 
  }
  Serial.println("LoRa Initialization OK!");
  display.setCursor(0,10);
  display.clearDisplay();
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);
}

void setWiFi(){
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
}


void sendReadings() { //send data with LoRa
  // 테스트 코드
//  value_co2 = (rand()%50)+700;
//  value_dust = (rand()%10)+30;
//  int mod = (rand()%100);
//  if(mod<50){ //3퍼센트의 확률로 이산화탄소랑 dust 농도 올라감 이때 많이 안올라갈수도있음.
//    value_co2+=(rand()%700);
//    value_dust+=(rand()%90);
//  }
  
  LoRaMessage = String(readingID) + "/" + String(value_co2) + "&" + String(value_dust);// + "#" + String(pressure);
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print("LoRa packet sent!");
  display.setCursor(0,20);
  display.print("CO2:");
  display.setCursor(72,20);
  display.print(value_co2);
  display.setCursor(0,30);
  display.print("Dust:");
  display.setCursor(54,30);
  display.print(value_dust);
  display.setCursor(0,40);
  display.print("Reading ID:");
  display.setCursor(66,50);
  display.print(readingID);
  display.display();
  Serial.print("Sending packet: ");
  Serial.println(readingID);
  readingID++;
}

void sensorTowifi(){ //read two seonsor data and request to Server
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
}

void setup() {
  Serial.begin(115200);//initialize Serial Monitor
  startOLED();   //startBME();
  startLoRA();
  pinMode(25, INPUT);  //Dust sensor
  //set WiFi
  setWiFi();
}
void loop() {
  srand(time(NULL));
  sendReadings();
  sensorTowifi();
  delay(3000);
}
