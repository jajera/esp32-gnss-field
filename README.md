# ESP32 GNSS field

A portable GNSS field node on Ubuntu 24.04: classic ESP32 + ATGM336H + ST7789 240×240 — wire it, flash phase folders, get a fix outdoors.

Docs: [esp32-gnss-field.johna.kiwi](https://esp32-gnss-field.johna.kiwi/) (after Pages + CNAME)

## Docs locally

```bash
cd ~/workspace/jajera/esp32-gnss-field
/usr/bin/python3.12 -m venv .venv
.venv/bin/python -m pip install -r requirements.txt
.venv/bin/python -m zensical serve
```

Open the URL printed by the server (usually `http://127.0.0.1:8000`).

Use `python -m` (not `.venv/bin/zensical` alone) so a broken script shebang cannot bite. Prefer `/usr/bin/python3.12` — a shell alias that points `python3` at 3.14 breaks venv creation.

## Start here

1. [docs/prerequisites.md](docs/prerequisites.md) — git, Python, PlatformIO
2. [docs/index.md](docs/index.md) — story + phases
3. [docs/firmware.md](docs/firmware.md) — frozen `firmware/<nn>-…/` apps

**Current:** v1 bring-up complete — Phases 0–7 passed on the bench (outdoor 3D fix on the field UI). See the [lab log](docs/lab-log.md).
