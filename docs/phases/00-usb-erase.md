# Phase 0 — USB, detect, erase

Get the PC talking to the board, prove it’s a classic ESP32, wipe flash so Phase 1 starts clean.

**Before this:** [Prerequisites](../prerequisites.md) — packages + PlatformIO installed (`pio` / `esptool` on your `PATH`).

## Firmware

Chip is empty after erase. No phase folder under `firmware/` yet — Phase 1 adds `01-hello/`. Tracker: [Firmware](../firmware.md).

```mermaid
flowchart LR
  PC[Linux PC] -->|data USB| CH340[CH340]
  CH340 --> ESP[ESP32]
  ESP --> Flash["4MB flash"]
```

---

## 0.1 Serial permissions (once per machine)

PlatformIO needs dialout + udev rules. From the **repo root**:

<div class="run" markdown>

```bash
sudo ./scripts/setup-linux-usb.sh
```

```text {.no-copy}
Installing udev rules → /etc/udev/rules.d/99-platformio-udev.rules
Reloading udev
Adding johna to group dialout

Setup complete.

Next steps (required):
  1. Log out and log back in (or reboot) so group membership applies.
  2. Unplug and replug the ESP32 USB cable.
  …
```

</div>

Log out/in (or reboot), then confirm:

<div class="run" markdown>

```bash
groups | grep dialout
test -f /etc/udev/rules.d/99-platformio-udev.rules && echo udev_ok
```

```text {.no-copy}
dialout
udev_ok
```

</div>

Don’t `sudo pio upload` — fix groups instead.

---

## 0.2 Plug the board

One USB socket on this classic DevKit (CH340 bridge). No COM / OTG choice like the S3 — plug the single port with a **data** cable.

![Classic ESP32 DevKit (CH340) — single USB port](../assets/esp32-board-top.jpg)

| Port | Role | This phase |
|------|------|------------|
| **USB** (micro / Type-C depending on board) | UART via CH340 → `/dev/ttyUSB*` | Plug here |

Power LED should come on. Expect `/dev/ttyUSB0` first; after unplug/replug the number can move (`ttyUSB1`, …) — always check with `ls`.

Buttons you’ll care about later: **BOOT** and **EN/RST**. If upload hangs on `Connecting…`, hold BOOT, tap EN, release BOOT. We didn’t need that here — esptool reset via RTS on its own.

---

## 0.3 See the serial device

If the host only shows root hubs, the cable is wrong or unplugged (charge-only cables never enumerate):

<div class="run" markdown>

```bash
lsusb
ls /dev/ttyACM* /dev/ttyUSB* 2>/dev/null
```

```text {.no-copy}
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 002 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
# no ttyACM / ttyUSB — try another data cable / USB port
```

</div>

When the board is right:

<div class="run" markdown>

```bash
lsusb
ls -la /dev/ttyUSB* /dev/serial/by-id/
```

```text {.no-copy}
Bus 001 Device 073: ID 1a86:7523 QinHeng Electronics CH340 serial converter
crw-rw-rw- 1 root dialout 188, 0 Oct 24 17:57 /dev/ttyUSB0
…/by-id/usb-1a86_USB_Serial-if00-port0 -> ../../ttyUSB0
```

</div>

That’s CH340 → `/dev/ttyUSB0` (or `ttyUSB1` after a later replug).

---

## 0.4 Chip and flash

### Chip

<div class="run" markdown>

```bash
esptool --port /dev/ttyUSB0 chip-id
```

```text {.no-copy}
esptool v5.3.0
Serial port /dev/ttyUSB0:
Connecting.......
Detecting chip type... ESP32
Connected to ESP32 on /dev/ttyUSB0:
Chip type:          ESP32-D0WD-V3 (revision v3.1)
Features:           Wi-Fi, BT, Dual Core + LP Core, 240MHz, …
Crystal frequency:  40MHz
MAC:                8c:94:df:6d:c9:5c

Warning: ESP32 has no chip ID. Reading MAC address instead.
MAC:                8c:94:df:6d:c9:5c

Hard resetting via RTS pin...
```

</div>

Classic **ESP32** (not S3). The “no chip ID” warning is normal — MAC is the identity you care about.

### Flash

<div class="run" markdown>

```bash
esptool --port /dev/ttyUSB0 flash-id
```

```text {.no-copy}
Detecting chip type... ESP32
…
Detected flash size: 4MB
```

</div>

| Item | This board |
|------|------------|
| Chip | ESP32-D0WD-V3 rev v3.1 |
| Flash | 4MB |
| MAC | `8C:94:DF:6D:C9:5C` |

---

## 0.5 Erase

!!! warning
    `erase-flash` wipes everything on the chip. Only run it once `/dev/ttyUSB*` is confirmed above — this is the clean start for Phase 1.

<div class="run" markdown>

```bash
esptool --port /dev/ttyUSB0 erase-flash
```

```text {.no-copy}
Erasing flash memory (this may take a while)...
Flash memory erased successfully in 12.2 seconds.
Hard resetting via RTS pin...
```

</div>

---

## Checkpoint 0

You’re done when:

- [x] `dialout` + udev rules in place
- [x] `lsusb` shows the bridge (`1a86:7523` here)
- [x] `/dev/ttyUSB*` exists and your user can open it
- [x] chip-id / flash-id say classic ESP32, 4MB flash
- [x] erase-flash finishes cleanly

Notes: [Lab log](../lab-log.md).

**Next:** [Phase 1 — Hello flash](01-hello-flash.md)
