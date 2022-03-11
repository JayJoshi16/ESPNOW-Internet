/* 

BLYNK ESP's Code. 

Paste this code in EXAMPLES > BLYNK > Blynk_edgent > Edgent_ESP32

This ESP will recieve JSON data from Coordinator ESP's and Sends it to BLYNK Cloud and vise versa.
Only this ESP is connected to Internet

*   Developed by  Jay Joshi 
*   github.com/JayJoshi16

*/
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_DEVICE_NAME "YOUR_DEVICE NAME"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial

#include "BlynkEdgent.h"

#include <ArduinoJson.h>

String recv_str_jsondata;

StaticJsonDocument<256> doc_send_v4;
StaticJsonDocument<256> doc_send_v5;        // creating diffrent doc for simplification
StaticJsonDocument<256> doc_recv;

#define RXD2 16
#define TXD2 17


BLYNK_WRITE(V4)                                 // Read from Virtual pin V4
{

  bool s4 = param.asInt();                    // parameter as int
  if (s4 == HIGH)
  {
    doc_send_v4["v4"] = "v4_on";                // Writing { v4 : v4_on } on Serial2
    Serial.println("Sended v4 : v4_on");
    serializeJson(doc_send_v4, Serial2);
    delay(10);
  }
  else if (s4 == LOW)
  {
    doc_send_v4["v4"] = "v4_off";                // Writing { v4 : v4_off } on Serial2
    Serial.println("Sended v4 : v4_off");
    serializeJson(doc_send_v4, Serial2);
    delay(10);
  }
}
BLYNK_WRITE(V5)                                   // Read from Virtual pin V5
{

  bool s5 = param.asInt();                         // parameter as int
  if (s5 == HIGH)
  {
    doc_send_v5["v5"] = "v5_on";                 // Writing { v5 : v5_on } on Serial2
    Serial.println("Sended v5 : v5_on");
    serializeJson(doc_send_v5, Serial2);
    delay(10);
  }
  else if (s5 == LOW)
  {
    doc_send_v5["v5"] = "v5_off";                  // Writing { v5 : v5_off } on Serial2
    Serial.println("Sended v5 : v5_off");
    serializeJson(doc_send_v5, Serial2);
    delay(10);
  }
}

void setup()
{
  BlynkEdgent.begin();

  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);        //Hardware Serial of ESP32

}

void loop() {

  BlynkEdgent.run();
  if (Serial2.available()) {                              // Recieving data (JSON) from Coordinator ESP
    
    recv_str_jsondata = Serial2.readStringUntil('\n');
    Serial.println(recv_str_jsondata);
    DeserializationError error = deserializeJson(doc_recv, recv_str_jsondata);

    if (!error) {                                           // if not error in deserialization
          float temp = doc_recv["v3"];                       // fetch temprature data from JSON . Here { v3 : 28.55 }  

          if (temp > 0) {
            Blynk.virtualWrite(V3, temp);                     // writing temprature to BLYNK Cloud 
            Serial.print("temp =");Serial.println(temp);
          }
    }

    else {                                     
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    recv_str_jsondata="";
  }

}
