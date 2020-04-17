#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <TinyGPS++.h>
#include "Ambient.h"

 extern "C" {
 #include "user_interface.h"
 }

 int xx=0;
 unsigned int channelId = 100;
 const char* writeKey = "ライトキー";

 const char* ssid = "...ssid...";
 const char* password = "...password...";

 static const int RXPin = 4, TXPin = 5;
// SoftwareSerial ss(RXPin, TXPin, false ,256);
 SoftwareSerial ss;
 TinyGPSPlus gps;
 void sendInfo2Ambient();
 static const int sw = 16;
 static const unsigned long PUSH_SHORT = 100;

 WiFiClient client;
 Ambient ambient;

 void setup()
 {
     Serial.begin(9600);
     delay(20);
     Serial.println("Start");

     wifi_set_sleep_type(LIGHT_SLEEP_T);
     WiFi.mode(WIFI_STA);
     WiFi.begin(ssid, password); // Wi-Fi APに接続
     while (WiFi.status() != WL_CONNECTED) { // Wi-Fi AP接続待ち
         delay(500);
     Serial.print(".");
     Serial.print(WiFi.status());
     }

     Serial.print("WiFi connected\r\nIP address: ");
     Serial.println(WiFi.localIP());

//     ss.begin(9600);
     ss.begin(9600, SWSERIAL_8N1,TXPin , RXPin, false, 256);
     pinMode(sw, INPUT_PULLUP);

     ambient.begin(channelId, writeKey, &client);
 }

 void loop()
 {
     unsigned long gauge = 0;
//     while (digitalRead(sw) == 0) {
//         gauge++;
//         delay(0);
//     }
//     if (gauge > PUSH_SHORT) {
         if (gps.location.isValid()) {
             sendInfo2Ambient();
         }
//     }
     while (ss.available() > 0) {
         if (gps.encode(ss.read())) {
             break;
         }
     }
delay(10000);
 }

 void sendInfo2Ambient()
 {
     char buf[16];

     if (gps.location.isValid()) {
         dtostrf(gps.altitude.meters(), 4, 2, buf);
         Serial.println(buf);
         ambient.set(1, buf);
         dtostrf(gps.location.lat(), 12, 8, buf);
         Serial.println(buf);
         ambient.set(9, buf);
         dtostrf(gps.location.lng(), 12, 8, buf);
         Serial.println(buf);
         ambient.set(10, buf);

         ambient.send();
     }
 }