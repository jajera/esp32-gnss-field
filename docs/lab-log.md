# Lab log

What actually passed on this bench. Detail lives in each [phase](phases/00-usb-erase.md); this page is the scoreboard.

## Bench

| Item | Value |
|------|-------|
| Host | Ubuntu 24.04 · CH340 `1a86:7523` · port moves (`/dev/ttyUSB0` / `ttyUSB1`) |
| Board | ESP32-D0WD-V3 · MAC `8C:94:DF:6D:C9:5C` |
| Stack | ATGM336H · ST7789 240×240 (TFT + GNSS power on) |
| On chip now | `firmware/07-field-ui` |

## Checkpoints

| # | Result | Notes |
|---|--------|-------|
| 0 | Pass | CH340 `/dev/ttyUSB0` · ESP32-D0WD-V3 rev v3.1 · MAC `8C:94:DF:6D:C9:5C` · erase ~12 s |
| 1 | Pass | build 2.3 s · upload ~20 s · serial hello + MAC `8C:94:DF:6D:C9:5C` · flash 4MB |
| 2 | Pass | TFT VCC/GND/BL → 3V3 star · hello OK after replug on `ttyUSB1` |
| 3 | Pass | GNSS VCC/GND + antenna · hello OK (watch heat — 3V3 only) |
| 4 | Pass | UART GPIO16←TX · `$GNGGA`/`$GNRMC`/`$BDGSV` · `ANTENNA OK` · no fix indoors (expected) |
| 5 | Pass | structured `fix=NONE sats=0/0 hdop=25.5` ~1 Hz indoors · 3D deferred to sky |
| 6 | Pass | SPI 18/23/5/21/22 · `init(240,240,SPI_MODE3)` · panel R→G→B |
| 7 | Pass | outdoor **FIX 3D** · SAT 8/10 · HDOP 2.3 · lat/lon locked · [demo photo](assets/07-field-ui-demo.jpg) |

## Quirks worth remembering

- Classic ESP32 here is **CH340 → `/dev/ttyUSB*`**, not the S3 dual Type-C COM jack from seismo. After unplug/replug the number can move (`ttyUSB0` → `ttyUSB1`); check `ls /dev/ttyUSB*` before monitor/upload.
- Cold start indoors can take minutes or never get a 3D fix — outdoor / clear sky is the real Phase 7 checkpoint.
- If NMEA looks like garbage, try baud **115200** instead of 9600.
- This ST7789 240×240 needed `tft.init(240, 240, SPI_MODE3)` (MODE0 stayed blank); don’t pass CS into `SPI.begin`.
