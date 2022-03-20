/* 

Coordiante ESP's Code. 
This ESP will recieve ESPNOW data from END Node ESP's and Serially trasnfers it to BLYNK ESP and vise versa.
MAC Address of this ESP is Provided to ESD Node ESP.

*   Developed by  Jay Joshi 
*   github.com/JayJoshi16
*/

#include <esp_now.h>
#include <WiFi.h>
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //Broadcast address

#include <SimpleTimer.h>
SimpleTimer timer;

#include <ArduinoJson.h>
StaticJsonDocument<256> doc_from_espnow;
StaticJsonDocument<256> doc_to_espnow;
String recv_jsondata;               // recieved JSON string
int temperature=0;

#define RXD2 16
#define TXD2 17        

#include <Wire.h>                     // oled
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  char* buff = (char*) incomingData;
  recv_jsondata = String(buff);
  Serial.print("Recieved from ESPNOW: "); Serial.println(recv_jsondata);
  DeserializationError error = deserializeJson(doc_from_espnow, recv_jsondata);
  if (!error) {
    Serial.print("Serilising to Serial2: ");
    Serial.println(recv_jsondata);
    temperature  = doc_from_espnow["v3"];                 // Data to be displayed on OLED
    timer.setTimeout(2000,displayText);                   // One Shot Timer
    
    serializeJson(doc_from_espnow, Serial2);            // Writing Data to Serial2
  }

  else {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

}

void displayText() {       // Function that Displays tempreature on OLED
  display.clearDisplay();
  display.setTextSize(1);    
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(2, 10);    
  display.print("Temperature :");
  display.setCursor(2, 40);
  display.print(temperature);
  display.display();
}

void setup() {

  //ONBOARD LED WILL GLOW IN CASE OF RESET
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  delay(2000);
  digitalWrite(2, LOW);
  delay(2000);

  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {                // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(200);
  display.display();
  display.clearDisplay();
  delay(200);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent); 
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

}

void loop() {
  timer.run();
  if (Serial2.available()) {                                      // Recieving data (JSON) from BLYNK ESP
    String recv_str_jsondata = Serial2.readStringUntil('\n');
    serializeJson(doc_to_espnow, recv_str_jsondata);              //Serilizing JSON
    Serial.println(recv_str_jsondata);
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) recv_str_jsondata.c_str(), sizeof(recv_str_jsondata) * recv_str_jsondata.length());
                                                        // Broadcasting data (JSON) to END Node ESP's via ESP-NOW
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println(result);
    }
    delay(20);
  }
}
