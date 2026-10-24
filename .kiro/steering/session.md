---
inclusion: always
---

# Session handoff (from seismo chat → this repo)

Context carried from the bring-up of `esp32-s3-seismo` and the decision to start this lab. Prefer this file + other `.kiro/steering/` over guessing.

## Repo

- Path: `~/workspace/jajera/esp32-gnss-field`
- GitHub: `jajera/esp32-gnss-field`
- Short desc (no special chars): `ESP32 portable GNSS field node with ATGM336H and ST7789`
- Docs site (when published): `esp32-gnss-field.johna.kiwi` (`docs/CNAME` already set)
- License + empty README existed; full Zensical/PlatformIO scaffold added locally — **no commit yet** unless John asks

## Hardware on the bench (detected)

- **Classic ESP32**, not S3: `ESP32-D0WD-V3` rev v3.1
- UART: CH340 `1a86:7523` → `/dev/ttyUSB0`
- MAC: `8C:94:DF:6D:C9:5C`
- GNSS: ATGM336H GPS+BDS (UART NMEA, often 9600; try 115200 if garbage). Listing checked: AliExpress ATGM336H / NEO-M8N-style pinout, PPS available
- Display: ST7789 **240×240** 8-pin SPI (`GND VCC SCL SDA RES DC CS BL`) — same silk family as seismo’s Estardyn, square IPS
- Power: **3V3 only** on GNSS and TFT — never 5V

## Use case (agreed)

**Portable GNSS field node** — Ubuntu lab, phased like seismo:

1. USB / hello  
2. TFT power (backlight)  
3. GNSS power + antenna  
4. Raw NMEA on serial  
5. Parsed fix (sats, HDOP, lat/lon) on serial  
6. TFT colour fill  
7. **Field UI** on TFT (v1 end): FIX / SAT / HDOP / LAT / LON / SPD / UTC, status colour red→yellow→green  

Not in v1: SoftAP, microSD log, PPS clock, NTRIP/RTK, SeisComP, AWS fleet. Those are later `firmware/<nn>-…/` folders only.

Learning demo, metre-class consumer GNSS — not a survey instrument.

## Lab conventions (mirror seismo, adapted)

- Docs: Zensical + patina; phases with checkpoints; `.run` command+result blocks
- **Diagrams: ASCII in markdown only** (no SVG, no mermaid for wiring/overview) — John requested this explicitly for this repo
- Firmware: frozen `firmware/<nn>-…/` snapshots; env `esp32dev`; port `/dev/ttyUSB0`
- Pin map (classic DevKit-class): UART2 RX**16**/TX**17**; TFT SCK**18** MOSI**23** CS**5** DC**21** RST**22**; BL→3V3 or **15**; PPS optional **4**
- Build anytime; **flash/erase only when asked** and a serial port is visible
- Update `docs/firmware.md` + phase Firmware blurbs when adding folders
- Lab log stays honest — don’t tick Pass without bench evidence

## Publish process (when John asks — same as seismo)

1. CNAME in `platformfuzz/johna-kiwi-infra` `sites.yaml` **first**, wait Terraform apply  
2. Issue + PR + wait checks + merge this repo  
3. Enable GitHub Pages (`build_type=workflow`) + custom domain; if cert stuck, recreate Pages domain  
4. Then human (not robotic) card in `jajera/guides` + `data/catalogue.json`  
5. Dependabot auto-merge via `actionsforge` reusable (workflow already scaffolded)  
6. Repo `allow_auto_merge` must be on for Dependabot merges  

Commit messages: conform policy (type, body required, no Cursor co-author trailers). Required PR workflows: markdown-lint + commitmsg-conform (individual actionsforge callers).

## Sibling context (do not mix)

- `esp32-s3-seismo` = ESP32-**S3** N16R8 + LSM6DSO + larger ST7789, MMI/STA-LTA desk seismo — finished v1, live at `esp32-s3-seismo.johna.kiwi`
- This repo = classic ESP32 + GNSS field node — separate product, separate publish

## Status at handoff

- Scaffold + bring-up docs phases 0–7; firmware 01/04/05/06/07; CI; steering  
- **Phases 0–6 Pass** on bench (classic ESP32 CH340, MAC `8C:94:DF:6D:C9:5C`)  
- **Phases 0–7 Pass** — v1 bring-up complete; `firmware/07-field-ui` on chip; outdoor **FIX 3D** (demo `docs/assets/07-field-ui-demo.jpg`)  
- Docs use mermaid + wiring SVGs to match live seismo; no commit until John asks  
- Next: commit/PR when asked; later SoftAP/SD/PPS/NTRIP as new `firmware/<nn>-…/` only
