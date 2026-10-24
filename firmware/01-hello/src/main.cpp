#include <Arduino.h>
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(500);
  WiFi.mode(WIFI_STA);
  delay(50);
  Serial.println();
  Serial.println("gnss-field hello — phase 1");
  Serial.printf("MAC=%s\n", WiFi.macAddress().c_str());
  Serial.printf("chip=ESP32 flash=%uMB\n",
                ESP.getFlashChipSize() / (1024u * 1024u));
}

void loop() {
  Serial.printf("gnss-field hello MAC=%s uptime_ms=%lu\n",
                WiFi.macAddress().c_str(),
                (unsigned long)millis());
  delay(1000);
}
