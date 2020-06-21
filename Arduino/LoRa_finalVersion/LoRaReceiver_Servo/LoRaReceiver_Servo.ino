#include <ESP32Servo.h>
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

// Initialize variables to get and save LoRa data
Servo servo;
int rssi;
String loRaMessage;
String CO2;
String Dust;
String readingID;
int CO2_led;
int Dust_led;
int servo_pin = 25;
int servo_angle = 0;

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
}


// Read LoRa packet and get the sensor readings
void getLoRaData() {
  Serial.print("Lora packet received: ");
  // Read packet
  while (LoRa.available()) {
    String LoRaData = LoRa.readString();
    Serial.print(LoRaData); 
    
    // Get readingID, temperature and soil moisture
    int pos1 = LoRaData.indexOf('/');
    int pos2 = LoRaData.indexOf('&');
    readingID = LoRaData.substring(0, pos1);
    CO2 = LoRaData.substring(pos1 +1, pos2);
    Dust = LoRaData.substring(pos2+1, LoRaData.length()); 
    display.clearDisplay();
    display.setCursor(0,10);
    display.print("CO2       : ");
    display.setCursor(70,10);
    display.print(CO2);
    display.setCursor(0,20); 
    display.print("Dust      : ");
    display.setCursor(70,20);
    display.print(Dust);
    display.setCursor(0,30);
    display.print("readingID : ");
    display.setCursor(70,30);
    display.print(readingID);
    display.display();
    Serial.println(CO2);
    Serial.println(Dust);
  }
  // Get RSSI
  rssi = LoRa.packetRssi();
  Serial.print(" with RSSI ");    
  Serial.println(rssi);
}

//String to int : String.toInt()   , int to String : String(int)
void moveServo(){
  CO2_led = CO2.toInt();
  Dust_led = Dust.toInt();
  
  if(CO2_led >= 1000 or Dust_led >= 70){ // 기준치 초과하는 경우
    if(servo_angle ==0){ // 창문이 닫혀있는 경우
      for(servo_angle; servo_angle < 46; servo_angle++){ // 45도 각도까지 개방
        servo.write(servo_angle);
      }
    }
  } else if(CO2_led <1000 && Dust_led < 70){ // 기준치 이하인 경우
    if(servo_angle != 0){// 창문이 닫혀있지 않으면
      for(servo_angle; servo_angle >=0; servo_angle--){ // 창문을 닫는다.
        servo.write(servo_angle);
      }
      servo_angle =0;
    } else{ // 창문이 닫혀있는 경우
      servo_angle = 0;
    }
  }
}

void setup() { 
  // Initialize Serial Monitor
  Serial.begin(115200);
  startOLED();
  startLoRA();
  servo.attach(servo_pin); // 핀 설정
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

void loop() {
  // Check if there are LoRa packets available
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    getLoRaData();
    moveServo();
 }
}
