#include <Arduino.h>

// Lab pin map — docs/hardware.md
static const int GNSS_RX = 16;  // ESP RX <- module TX
static const int GNSS_TX = 17;  // ESP TX -> module RX
static const uint32_t GNSS_BAUD = 9600;

HardwareSerial GnssSerial(2);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("gnss-field nmea-raw — phase 4");
  Serial.printf("UART2 RX=%d TX=%d baud=%lu\n", GNSS_RX, GNSS_TX,
                (unsigned long)GNSS_BAUD);
  GnssSerial.begin(GNSS_BAUD, SERIAL_8N1, GNSS_RX, GNSS_TX);
}

void loop() {
  while (GnssSerial.available()) {
    Serial.write(GnssSerial.read());
  }
}
