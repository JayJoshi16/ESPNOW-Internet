/*  
Code for END NODE ESP 2
This will RECIEVE data from Coordinator ESP using ESP-NOW and Toggle BuiltIn LED
*  Developed by  Jay Joshi 
*  github.com/JayJoshi16
*/

#include <esp_now.h>
#include <WiFi.h>

#include <ArduinoJson.h>

String recv_jsondata;

StaticJsonDocument<256> doc_from_espnow;  

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  char* buff = (char*) incomingData;
  recv_jsondata = String(buff);
  Serial.print("Recieved ");Serial.println(recv_jsondata);
  DeserializationError error = deserializeJson(doc_from_espnow, recv_jsondata);
  
  if (!error) {
    const char* led_status   = doc_from_espnow["v5"];        //fetching values of v5 from doc_from_espnow JSON
    
    if(strstr(led_status,"v5_on"))
    digitalWrite(2,HIGH);                            // OnBoard LED HIGH   

    else if(strstr(led_status,"v5_off"))
    digitalWrite(2,LOW);                              // OnBoard LED LOW

    led_status="";
  }

  else {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

}

void setup() {
  
  //ONBOARD LED WILL GLOW IN CASE OR RESET
  pinMode(2,OUTPUT);

  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
 
  esp_now_register_recv_cb(OnDataRecv);
  
}

void loop() {
 
}
