#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <stdlib.h>
#include <string.h>

static const int GNSS_RX = 16;
static const int GNSS_TX = 17;
static const uint32_t GNSS_BAUD = 9600;

static const int TFT_SCK = 18;
static const int TFT_MOSI = 23;
static const int TFT_CS = 5;
static const int TFT_DC = 21;
static const int TFT_RST = 22;

HardwareSerial GnssSerial(2);
Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, TFT_CS, TFT_DC, TFT_RST);

enum FixKind { FIX_NONE = 0, FIX_2D = 1, FIX_3D = 2 };

struct GnssState {
  FixKind fix = FIX_NONE;
  int sats_used = 0;
  int sats_view = 0;
  float hdop = 99.9f;
  double lat = 0.0;
  double lon = 0.0;
  float speed_kn = 0.0f;
  char utc[16] = "";
};

static GnssState g;
static bool dirty = true;

static double nmeaToDeg(const char *ddmm, char hemi) {
  if (!ddmm || !*ddmm) {
    return 0.0;
  }
  double v = atof(ddmm);
  int deg = (int)(v / 100.0);
  double minutes = v - (deg * 100.0);
  double out = deg + minutes / 60.0;
  if (hemi == 'S' || hemi == 'W') {
    out = -out;
  }
  return out;
}

static int field(char *line, int idx, char *out, size_t out_len) {
  char *p = line;
  int n = 0;
  while (*p && n < idx) {
    if (*p++ == ',') {
      n++;
    }
  }
  if (n != idx) {
    out[0] = '\0';
    return 0;
  }
  size_t i = 0;
  while (*p && *p != ',' && *p != '*' && i + 1 < out_len) {
    out[i++] = *p++;
  }
  out[i] = '\0';
  return 1;
}

static void handleSentence(char *line) {
  char *star = strchr(line, '*');
  if (star) {
    *star = '\0';
  }

  if (strstr(line, "GGA")) {
    char buf[32], ns[4], ew[4], q[4], sats[8], hdop[8];
    field(line, 1, g.utc, sizeof(g.utc));
    field(line, 2, buf, sizeof(buf));
    field(line, 3, ns, sizeof(ns));
    g.lat = nmeaToDeg(buf, ns[0]);
    field(line, 4, buf, sizeof(buf));
    field(line, 5, ew, sizeof(ew));
    g.lon = nmeaToDeg(buf, ew[0]);
    field(line, 6, q, sizeof(q));
    field(line, 7, sats, sizeof(sats));
    field(line, 8, hdop, sizeof(hdop));
    int qi = atoi(q);
    g.fix = (qi == 0) ? FIX_NONE : FIX_3D;
    g.sats_used = atoi(sats);
    g.hdop = atof(hdop);
    if (g.hdop <= 0.0f) {
      g.hdop = 99.9f;
    }
    dirty = true;
  } else if (strstr(line, "RMC")) {
    char buf[32], st[4], ns[4], ew[4], spd[16];
    field(line, 1, g.utc, sizeof(g.utc));
    field(line, 2, st, sizeof(st));
    field(line, 3, buf, sizeof(buf));
    field(line, 4, ns, sizeof(ns));
    if (st[0] == 'A') {
      g.lat = nmeaToDeg(buf, ns[0]);
      field(line, 5, buf, sizeof(buf));
      field(line, 6, ew, sizeof(ew));
      g.lon = nmeaToDeg(buf, ew[0]);
      field(line, 7, spd, sizeof(spd));
      g.speed_kn = atof(spd);
      dirty = true;
    }
  } else if (strstr(line, "GSV")) {
    char total[8];
    field(line, 3, total, sizeof(total));
    int v = atoi(total);
    if (v > g.sats_view) {
      g.sats_view = v;
      dirty = true;
    }
  }
}

static void pollGnss() {
  static char line[128];
  static size_t len = 0;
  while (GnssSerial.available()) {
    char c = (char)GnssSerial.read();
    if (c == '\n' || c == '\r') {
      if (len > 0) {
        line[len] = '\0';
        if (line[0] == '$') {
          handleSentence(line);
        }
        len = 0;
      }
    } else if (len + 1 < sizeof(line)) {
      line[len++] = c;
    } else {
      len = 0;
    }
  }
}

static const char *fixName(FixKind f) {
  switch (f) {
    case FIX_2D:
      return "2D";
    case FIX_3D:
      return "3D";
    default:
      return "NONE";
  }
}

static uint16_t statusColor() {
  if (g.fix == FIX_NONE) {
    return ST77XX_RED;
  }
  if (g.fix == FIX_2D || g.hdop > 2.5f) {
    return ST77XX_YELLOW;
  }
  return ST77XX_GREEN;
}

static void drawUi() {
  uint16_t col = statusColor();
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRect(0, 0, 240, 28, col);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(8, 6);
  tft.print("FIX ");
  tft.print(fixName(g.fix));

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(8, 40);
  tft.printf("SAT  %d/%d", g.sats_used, g.sats_view);
  tft.setCursor(8, 64);
  tft.printf("HDOP %.1f", g.hdop);
  tft.setCursor(8, 96);
  tft.printf("LAT  %.5f", g.lat);
  tft.setCursor(8, 120);
  tft.printf("LON  %.5f", g.lon);
  tft.setCursor(8, 152);
  tft.printf("SPD  %.1f kn", g.speed_kn);
  tft.setCursor(8, 184);
  tft.printf("UTC  %s", g.utc[0] ? g.utc : "--");
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("gnss-field field-ui — phase 7");

  GnssSerial.begin(GNSS_BAUD, SERIAL_8N1, GNSS_RX, GNSS_TX);
  SPI.begin(TFT_SCK, /*MISO*/ -1, TFT_MOSI, /*SS*/ -1);
  tft.init(240, 240, SPI_MODE3);
  tft.setSPISpeed(20000000);
  tft.setRotation(0);
  drawUi();
}

void loop() {
  pollGnss();
  static uint32_t last = 0;
  if (dirty || millis() - last >= 1000) {
    dirty = false;
    last = millis();
    drawUi();
    Serial.printf("fix=%s sats=%d/%d hdop=%.1f lat=%.6f lon=%.6f\n",
                  fixName(g.fix), g.sats_used, g.sats_view, g.hdop, g.lat, g.lon);
  }
}
