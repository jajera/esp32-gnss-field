#include <Arduino.h>
#include <stdlib.h>
#include <string.h>

static const int GNSS_RX = 16;
static const int GNSS_TX = 17;
static const uint32_t GNSS_BAUD = 9600;

HardwareSerial GnssSerial(2);

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
  // idx is 0-based field after the talker ($GNGGA fields start at 0 = sentence)
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
  // Strip checksum / CR
  char *star = strchr(line, '*');
  if (star) {
    *star = '\0';
  }

  if (strstr(line, "GGA")) {
    char buf[32];
    field(line, 1, g.utc, sizeof(g.utc));
    field(line, 2, buf, sizeof(buf));
    char ns[4], ew[4], q[4], sats[8], hdop[8];
    field(line, 3, ns, sizeof(ns));
    g.lat = nmeaToDeg(buf, ns[0]);
    field(line, 4, buf, sizeof(buf));
    field(line, 5, ew, sizeof(ew));
    g.lon = nmeaToDeg(buf, ew[0]);
    field(line, 6, q, sizeof(q));
    field(line, 7, sats, sizeof(sats));
    field(line, 8, hdop, sizeof(hdop));
    int qi = atoi(q);
    g.fix = (qi == 0) ? FIX_NONE : (qi >= 1 ? FIX_3D : FIX_NONE);
    // GGA quality 1=GPS, 2=DGPS — treat as 3D-ish; refine with GSA later
    if (qi == 0) {
      g.fix = FIX_NONE;
    } else {
      g.fix = FIX_3D;
    }
    g.sats_used = atoi(sats);
    g.hdop = atof(hdop);
    if (g.hdop <= 0.0f) {
      g.hdop = 99.9f;
    }
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
    }
  } else if (strstr(line, "GSV")) {
    char total[8];
    field(line, 3, total, sizeof(total));
    int v = atoi(total);
    if (v > g.sats_view) {
      g.sats_view = v;
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

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("gnss-field fix-parse — phase 5");
  Serial.printf("UART2 RX=%d TX=%d baud=%lu\n", GNSS_RX, GNSS_TX,
                (unsigned long)GNSS_BAUD);
  GnssSerial.begin(GNSS_BAUD, SERIAL_8N1, GNSS_RX, GNSS_TX);
}

void loop() {
  pollGnss();
  static uint32_t last = 0;
  if (millis() - last >= 1000) {
    last = millis();
    Serial.printf("fix=%s sats=%d/%d hdop=%.1f lat=%.6f lon=%.6f spd=%.1f utc=%s\n",
                  fixName(g.fix), g.sats_used, g.sats_view, g.hdop, g.lat, g.lon,
                  g.speed_kn, g.utc);
  }
}
