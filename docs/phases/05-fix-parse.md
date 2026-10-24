# Phase 5 — Fix parse

Parse GGA/RMC (and friends) into fix state, sats, HDOP, lat/lon. Still serial-only — TFT comes next. No new wires.

```mermaid
flowchart LR
  NMEA[NMEA stream] --> Parser
  Parser --> Lines["FIX / sats / HDOP / lat lon"]
  Lines -->|115200| USB[ttyUSB0]
```

## Firmware

**Changed.** New folder [`firmware/05-fix-parse/`](../../firmware/05-fix-parse/). Leave earlier phase folders untouched. Same UART2 `RX=16` `TX=17` baud **9600**.

## Preconditions

- [Checkpoint 4](04-nmea-raw.md) passed (raw NMEA visible)
- Phases 2–4 wiring still on

---

## 5.1 Flash

Stop any serial monitor on the port first. Check which `/dev/ttyUSB*` is up.

<div class="run" markdown>

```bash
ls /dev/ttyUSB*
cd firmware/05-fix-parse
pio run -e esp32dev -t upload --upload-port /dev/ttyUSB0
```

```text {.no-copy}
/dev/ttyUSB0
Processing esp32dev (platform: espressif32; board: esp32dev; framework: arduino)
--------------------------------------------------------------------------------
Verbose mode can be enabled via `-v, --verbose` option
CONFIGURATION: https://docs.platformio.org/page/boards/espressif32/esp32dev.html
PLATFORM: Espressif 32 (55.3.39) > Espressif ESP32 Dev Module
HARDWARE: ESP32 240MHz, 320KB RAM, 4MB Flash
DEBUG: Current (cmsis-dap) External (cmsis-dap, esp-bridge, esp-prog, esp-prog-2, iot-bus-jtag, jlink, minimodule, olimex-arm-usb-ocd, olimex-arm-usb-ocd-h, olimex-arm-usb-tiny-h, olimex-jtag-tiny, tumpa)
PACKAGES: 
 - contrib-piohome @ 3.4.4 
 - framework-arduinoespressif32 @ 3.3.9 
 - framework-arduinoespressif32-libs @ 5.5.4+sha.735507283d 
 - tool-esptoolpy @ 5.3.0 
 - tool-xtensa-esp-elf-gdb @ 17.1.0+20260402 
 - toolchain-xtensa-esp-elf @ 14.2.0+20260121
LDF: Library Dependency Finder -> https://bit.ly/configure-pio-ldf
LDF Modes: Finder ~ chain, Compatibility ~ soft
Found 43 compatible libraries
Scanning dependencies...
No dependencies
Building in release mode
Retrieving maximum program size .pio/build/esp32dev/firmware.elf
Checking size .pio/build/esp32dev/firmware.elf
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [=         ]   6.8% (used 22348 bytes from 327680 bytes)
Flash: [==        ]  22.7% (used 297440 bytes from 1310720 bytes)
Configuring upload protocol...
AVAILABLE: cmsis-dap, esp-bridge, esp-prog, esp-prog-2, espota, esptool, iot-bus-jtag, jlink, minimodule, olimex-arm-usb-ocd, olimex-arm-usb-ocd-h, olimex-arm-usb-tiny-h, olimex-jtag-tiny, tumpa
CURRENT: upload_protocol = esptool
Looking for upload port...
Uploading .pio/build/esp32dev/firmware.bin
esptool v5.3.0
Serial port /dev/ttyUSB0:
Connecting....
Connected to ESP32 on /dev/ttyUSB0:
Chip type:          ESP32-D0WD-V3 (revision v3.1)
Features:           Wi-Fi, BT, Dual Core + LP Core, 240MHz, Vref calibration in eFuse, Coding Scheme None
Crystal frequency:  40MHz
MAC:                8c:94:df:6d:c9:5c

Uploading stub flasher...
Running stub flasher...
Stub flasher running.
Changing baud rate to 460800...
Changed.

Configuring flash size...
Auto-detected flash size: 4MB

Writing '/home/johna/workspace/jajera/esp32-gnss-field/firmware/05-fix-parse/.pio/build/esp32dev/bootloader.bin' at 0x00001000...
SHA digest in image updated.
Flash will be erased from 0x00001000 to 0x00006fff...
Compressed 23552 bytes to 15127...

Writing at 0x00001000 [░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░]   0.0% 0/15127 bytes... 

Writing at 0x00006c00 [██████████████████████████████] 100.0% 15127/15127 bytes... 
Wrote 23552 bytes (15127 compressed) at 0x00001000 in 0.7 seconds (254.9 kbit/s).
Verifying written data...
Hash of data verified.

Writing '/home/johna/workspace/jajera/esp32-gnss-field/firmware/05-fix-parse/.pio/build/esp32dev/partitions.bin' at 0x00008000...
Flash will be erased from 0x00008000 to 0x00008fff...
Compressed 3072 bytes to 146...

Writing at 0x00008000 [░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░]   0.0% 0/146 bytes... 

Writing at 0x00008c00 [██████████████████████████████] 100.0% 146/146 bytes... 
Wrote 3072 bytes (146 compressed) at 0x00008000 in 0.1 seconds (365.1 kbit/s).
Verifying written data...
Hash of data verified.

Writing '/home/johna/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin' at 0x0000e000...
Flash will be erased from 0x0000e000 to 0x0000ffff...
Compressed 8192 bytes to 47...

Writing at 0x0000e000 [░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░]   0.0% 0/47 bytes... 

Writing at 0x00010000 [██████████████████████████████] 100.0% 47/47 bytes... 
Wrote 8192 bytes (47 compressed) at 0x0000e000 in 0.1 seconds (474.1 kbit/s).
Verifying written data...
Hash of data verified.

Writing '.pio/build/esp32dev/firmware.bin' at 0x00010000...
Flash will be erased from 0x00010000 to 0x00058fff...
Compressed 297840 bytes to 164422...

Writing at 0x00010000 [░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░]   0.0% 0/164422 bytes... 
...
Writing at 0x00058b70 [██████████████████████████████] 100.0% 164422/164422 bytes... 
Wrote 297840 bytes (164422 compressed) at 0x00010000 in 4.3 seconds (548.1 kbit/s).
Verifying written data...
Hash of data verified.

Hard resetting via RTS pin...
========================= [SUCCESS] Took 14.42 seconds =========================
```

</div>

<div class="run" markdown>

```bash
pio device monitor -b 115200 --port /dev/ttyUSB0
```

```text {.no-copy}
--- Terminal on /dev/ttyUSB0 | 115200 8-N-1
--- Available filters and text transformations: debug, default, direct, esp32_exception_decoder, hexlify, log2file, nocontrol, printable, send_on_enter, time
--- More details at https://bit.ly/pio-monitor-filters
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H
gnss-field fix-parse — phase 5
UART2 RX=16 TX=17 baud=9600
fix=NONE sats=0/0 hdop=99.9 lat=0.000000 lon=0.000000 spd=0.0 utc=
fix=NONE sats=0/0 hdop=25.5 lat=0.000000 lon=0.000000 spd=0.0 utc=
fix=NONE sats=0/0 hdop=25.5 lat=0.000000 lon=0.000000 spd=0.0 utc=
fix=NONE sats=0/0 hdop=25.5 lat=0.000000 lon=0.000000 spd=0.0 utc=
fix=NONE sats=0/0 hdop=25.5 lat=0.000000 lon=0.000000 spd=0.0 utc=
fix=NONE sats=0/0 hdop=25.5 lat=0.000000 lon=0.000000 spd=0.0 utc=
fix=NONE sats=0/0 hdop=25.5 lat=0.000000 lon=0.000000 spd=0.0 utc=
```

</div>

Print ~1 Hz. Indoors `fix=NONE` with lat/lon zero is normal (matches Phase 4 empty GGA). Window or outdoors for `2D` / `3D` and real coordinates.

---

## Checkpoint 5

- [x] Structured fix lines on serial (even if `NONE` indoors)
- [ ] Outdoors or window: fix advances toward 3D when sky is open *(deferred — fair sky later)*
- [x] Logged in [Lab log](../lab-log.md)

**Next:** [Phase 6 — TFT fill](06-tft-fill.md)
