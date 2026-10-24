# Agent notes

GNSS field lab: classic ESP32 + ATGM336H + ST7789 240×240. Docs in Zensical. Firmware in PlatformIO Arduino. Demo host: Ubuntu 24.04.

Read `.kiro/steering/` (especially **`session.md`**) and `.cursor/rules/` first. Assume a clean machine until [docs/prerequisites.md](docs/prerequisites.md) is done (`pio` is not magic).

```bash
# after prerequisites — firmware
cd firmware/01-hello && pio run -e esp32dev

# walkthrough authors only (not lab steps)
python3 -m venv .venv && .venv/bin/pip install -r requirements.txt
.venv/bin/zensical serve
```

One phase at a time. Pass the checkpoint before the next. New app behaviour = new `firmware/<nn>-…/` folder. Update `docs/firmware.md`. Don’t flash unless asked and a serial port is there.

Diagrams in docs: **ASCII in markdown** (no SVG/mermaid for wiring/overview).
