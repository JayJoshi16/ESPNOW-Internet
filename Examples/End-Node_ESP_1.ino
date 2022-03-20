/*  
Code for END NODE ESP1 
This ESP will SEND Temprature to Coordinator ESP using ESP-NOW
Also it will RECIEVE data from Coordinator ESP using ESP-NOW

*  Developed by  Jay Joshi 
*  github.com/JayJoshi16

*/
#include <esp_now.h>
#include <WiFi.h>
uint8_t broadcastAddress[] = {0x3C, 0x71, 0xBF, 0x4C, 0x9C, 0xD0}; //Coordinator ESP MAC address

#include <DHT.h>
#define DHT11PIN 5                  // DHT11 on GPIO 5
DHT dht(DHT11PIN, DHT11);

#include <ArduinoJson.h>
String recv_jsondata;
String send_jsondata;
StaticJsonDocument<256> doc_to_espnow;
StaticJsonDocument<256> doc_from_espnow;  

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  char* buff = (char*) incomingData;
  recv_jsondata = String(buff);
  Serial.print("Recieved ");Serial.println(recv_jsondata);
  DeserializationError error = deserializeJson(doc_from_espnow, recv_jsondata);
  if (!error) {
    String led_status   = doc_from_espnow["v4"];       //fetching values of v4 from doc_from_espnow JSON
    
    if(led_status=="v4_on")
    digitalWrite(2,HIGH);                                 // OnBoard LED HIGH
    
    else if(led_status=="v4_off")
    digitalWrite(2,LOW);                                  // OnBoard LED LOW
    else
    Serial.println(led_status);
  }
  else {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
}


void setup() {
  
  dht.begin();
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);             
  esp_now_register_recv_cb(OnDataRecv);
  
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
                                                  // Sending Tempreature Every 4 Seconds
  float temp = dht.readTemperature();
  doc_to_espnow["v3"] = temp;                       // Creating JSON data. Here { v3 : 28.55 }
  serializeJson(doc_to_espnow, send_jsondata);
  esp_now_send(broadcastAddress, (uint8_t *) send_jsondata.c_str(), sizeof(send_jsondata)*send_jsondata.length());
                                                    // Sending it to Coordinater ESP
  Serial.println(send_jsondata); 
  send_jsondata = "";
  delay(4000);
}
