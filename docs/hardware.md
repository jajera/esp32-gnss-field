# Hardware reference

## Parts

| Role | Part | Notes |
|------|------|-------|
| MCU | ESP32 DevKit-class | Classic ESP32, CH340 → `/dev/ttyUSB0` |
| GNSS | ATGM336H | GPS+BDS, UART NMEA; external or onboard antenna |
| Display | ST7789 240×240 | 8-pin SPI IPS |
| Breadboard | Any solderless | Half-size 400 or full 830 |
| Jumpers | DuPont | M–M for board↔board / board↔rail |

## Breadboard (default connectivity)

Nothing is “wired” yet — the plastic board already has metal clips under the holes. Know that map **before** Phase 2, or a star becomes accidental shorts.

| Region | How holes are tied (factory default) |
|--------|--------------------------------------|
| Terminal strips | Each **row** of five (`a–e` or `f–j`) is one net. Same-number left and right are **not** joined — the centre trench isolates them. |
| Power rails | The `+` column is one vertical net; the `−` column is another. Red/`+` → **3V3**, blue/`−` → **GND** in this lab. |
| Rail gaps | Many boards break each rail halfway. Bridging the gap with a short jumper makes one continuous rail. |

Pick one `+` rail and one `−` rail as the 3V3/GND star. Run ESP32 **3V3** and **GND** into those rails once, then tap modules from the rails — don’t daisy-chain module-to-module for power.

## Wiring (pad-to-pad)

```text
ESP32                 ATGM336H                 ST7789 240x240
-----                 --------                 --------------
3V3        ---------> VCC                      VCC
GND        ---------> GND                      GND
GPIO16     <--------- TX     (ESP RX)
GPIO17     ---------> RX     (ESP TX; optional early)
GPIO4      <--------- PPS    (optional; leave open in v1)

GPIO18     ----------------------------------> SCL (SCK)
GPIO23     ----------------------------------> SDA (MOSI)
GPIO5      ----------------------------------> CS
GPIO21     ----------------------------------> DC
GPIO22     ----------------------------------> RES
GPIO15     -optional------------------------> BL
                       (or tie BL to 3V3)
```

### Rules

- **3V3 only** on GNSS and TFT — never 5V logic
- Do **not** join TFT SDA to anything on the UART side
- GNSS needs sky view (or a clear window) for a real fix
- Avoid strap pins **0 / 2 / 12** for SPI clock/data unless you know the board boot straps
- Confirm TFT silk: `GND · VCC · SCL · SDA · RES · DC · CS · BL` (left→right on many 8-pin boards)
- ATGM336H silk often: `VCC · GND · TX · RX · PPS`

## Baud

ATGM336H modules commonly ship at **9600** NMEA. Some are **115200**. Phase 4 docs assume 9600; change `GNSS_BAUD` in firmware if your lines look like noise.
