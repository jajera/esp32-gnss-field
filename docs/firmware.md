# Firmware

Each phase that needs an app gets its **own folder** under [`firmware/`](../firmware/). That folder is a frozen snapshot: later phases add a new directory instead of rewriting an old one.

```text
firmware/
  01-hello/          # Phase 1 (+ kept for Phases 2–3)
  04-nmea-raw/       # Phase 4
  05-fix-parse/      # Phase 5
  06-tft-fill/       # Phase 6
  07-field-ui/       # Phase 7
```

Phase 0 has no app (erased chip). Phases 2–3 keep flashing `01-hello` unchanged.

---

## On the board now

**On chip now:** `firmware/07-field-ui` — live GNSS dashboard on the TFT (Checkpoint 7 Pass).

| Setting | Value |
|---------|-------|
| Env | `esp32dev` |
| Port | `/dev/ttyUSB0` (CH340 `1a86:7523`) |
| MAC (bench) | `8C:94:DF:6D:C9:5C` |

Sources: see each folder’s `src/main.cpp` · `platformio.ini`.

---

## Build and flash

Same pattern for every phase: `cd` into the phase folder, build with env `esp32dev`, then upload.

<div class="run" markdown>

```bash
cd firmware/01-hello
pio run -e esp32dev
```

```text {.no-copy}
RAM:   [==        ]  …
Flash: [==        ]  …
========================= [SUCCESS] Took … seconds =========================
```

</div>

<div class="run" markdown>

```bash
pio run -e esp32dev -t upload --upload-port /dev/ttyUSB0
```

```text {.no-copy}
Connected to ESP32 on /dev/ttyUSB0:
…
Hash of data verified.
Hard resetting via RTS pin...
========================= [SUCCESS] Took … seconds =========================
```

</div>

Serial: [Phase 1, section 1.3](phases/01-hello-flash.md#13-serial).

---

## Stage map

| Phase | Folder | On the chip |
|-------|--------|-------------|
| 0 | — | empty (erased) |
| **1** | **`01-hello`** | hello |
| 2 | `01-hello` | same image, not edited |
| 3 | `01-hello` | same image, not edited |
| 4 | `04-nmea-raw` | raw NMEA mirror |
| 5 | `05-fix-parse` | parsed fix on serial |
| 6 | `06-tft-fill` | TFT SPI fill |
| **7** | **`07-field-ui`** | GNSS field UI |

## Change log

- **Phase 0** — erase; no app
- **Phase 1** — added `firmware/01-hello/`
- **Phase 4** — added `firmware/04-nmea-raw/`
- **Phase 5** — added `firmware/05-fix-parse/`
- **Phase 6** — added `firmware/06-tft-fill/`
- **Phase 7** — added `firmware/07-field-ui/`
