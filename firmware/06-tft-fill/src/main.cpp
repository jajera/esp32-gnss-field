#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// Lab pin map — docs/hardware.md
static const int TFT_SCK = 18;
static const int TFT_MOSI = 23;
static const int TFT_CS = 5;
static const int TFT_DC = 21;
static const int TFT_RST = 22;
// BL tied to 3V3 in Phase 2 (or GPIO15)

// Hardware SPI on VSPI pins; pass &SPI so begin() uses our pin map.
Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("gnss-field tft-fill — phase 6");
  Serial.printf("SPI SCK=%d MOSI=%d CS=%d DC=%d RST=%d\n", TFT_SCK, TFT_MOSI,
                TFT_CS, TFT_DC, TFT_RST);

  // Do not pass CS into SPI.begin — Adafruit toggles CS itself.
  SPI.begin(TFT_SCK, /*MISO*/ -1, TFT_MOSI, /*SS*/ -1);

  // Cheap 240x240 modules often need MODE2/MODE3 (not MODE0).
  tft.init(240, 240, SPI_MODE3);
  tft.setSPISpeed(20000000);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
  Serial.println("init 240x240 SPI_MODE3 @20MHz");
  Serial.println("cycling red → green → blue every 1 s");
  Serial.println("if panel blank: recheck SCL/SDA/CS/DC/RES; try MODE2 in main.cpp");
}

void loop() {
  tft.fillScreen(ST77XX_RED);
  Serial.println("fill RED");
  delay(1000);
  tft.fillScreen(ST77XX_GREEN);
  Serial.println("fill GREEN");
  delay(1000);
  tft.fillScreen(ST77XX_BLUE);
  Serial.println("fill BLUE");
  delay(1000);
}
