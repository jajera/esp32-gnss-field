# Firmware

One frozen app per phase folder. See [docs/firmware.md](../docs/firmware.md).

```bash
cd firmware/01-hello
pio run -e esp32dev
pio run -e esp32dev -t upload --upload-port /dev/ttyUSB0
```
