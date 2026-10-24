---
inclusion: always
---

# Tech

Demo is written for Ubuntu 24.04. Fresh machine: follow `docs/prerequisites.md` before any phase (apt → PlatformIO → `PATH`). Don’t assume `pio` exists.

Docs: Zensical + patina. Firmware: PlatformIO Arduino, env `esp32dev` (classic ESP32, CH340 → `/dev/ttyUSB0`).

Pins we use (classic DevKit-class):

- GNSS UART2: RX **16** (from module TX), TX **17** (to module RX)
- PPS optional: **4** (leave open until a later phase)
- TFT SPI: SCK **18**, MOSI **23**, CS **5**, DC **21**, RST **22**, BL to 3V3 or **15**
- Avoid GPIO **0 / 2 / 12 / 15** for strap-sensitive boot unless you know the board
- Module + TFT logic: **3V3 only** — never 5V

```bash
.venv/bin/zensical serve
cd firmware/01-hello && pio run -e esp32dev
pio run -e esp32dev -t upload --upload-port /dev/ttyUSB0
```

Lab board MAC (bench): `8C:94:DF:6D:C9:5C`.
