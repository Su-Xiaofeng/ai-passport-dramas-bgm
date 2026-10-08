<p align="right"><a href="README.zh_CN.md">简体中文</a> · <strong>English</strong></p>

# Queen Reborn BGM

An offline six-track BGM player for FoloToy AI Passport, built from
[FoloToy/ai-passport](https://github.com/FoloToy/ai-passport).
Boot opens the black-and-gold 240 x 320 player, paused at 45% volume with
single-track repeat enabled. The screen shows the track, progress, volume,
battery and playback mode. Music is embedded in the firmware.

## Controls

| Button | Click | Hold | Double click |
| --- | --- | --- | --- |
| UP | Previous track | Volume +10 | No action |
| DOWN | Next track | Volume -10 | No action |
| OK | Play/pause | Toggle repeat / play once | No action |

Track selection wraps around; pausing retains position. No demo menu is available.
The project title uses a 16-pixel font, and the hold instructions share one row.

## Build and flash

Target: ESP32-C3, 8 MB Flash, no PSRAM, ESP-IDF 5.5.3.
Activate ESP-IDF and install FFmpeg for the audio tests and packer.

```bash
python3 tools/pack_bgm.py --require-all
./tools/validate.sh
```

Flash the verified merged `build/FoloToy-AI-Passport-full.bin` at `0x0`.
The six WAV excerpts and generated audio/font sources are included for
reproducible builds. Build products, local downloads and debug logs are ignored.

## Documentation

- [Player architecture, UI and validation](docs/applications/queen-bgm.md)
- [Six music sources and excerpt lengths](assets/music/README.md)
- [Community publishing materials](docs/applications/queen-bgm-publishing.md)
- [Hardware and development guides](docs/README.md)

## Licenses

Code uses the [MIT license](LICENSE). Noto Sans CJK fonts use
[SIL Open Font License 1.1](assets/fonts/OFL.txt).
Music redistribution permission is unverified; the code license does not
grant a license for the bundled recordings.
