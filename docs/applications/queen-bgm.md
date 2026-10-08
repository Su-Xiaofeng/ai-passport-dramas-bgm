<p align="right"><a href="queen-bgm.zh_CN.md">简体中文</a> · <strong>English</strong></p>

# Queen Reborn BGM

The player starts directly on its black-and-gold 240 x 320 screen. The shared
project title is defined in `main/queen_bgm.h`. Startup defaults are paused,
track 1, volume 45% and single-track repeat. No demo menu or return action is
present. Title font size is 16 pixels; hold instructions occupy one row.

## Playback behavior

UP/DOWN selects the previous/next track with wraparound; holding adjusts volume
by 10 within 0-100%. OK plays/pauses while retaining position; holding OK toggles
repeat/play once. Double clicks have no action. Switching tracks while playing
starts the new track from its beginning. Playback settings reset on boot.

After 20 seconds without input the backlight dims to 15%; input restores 70%.
Pausing sleeps the codec, and resuming safely restores its playback format.
The player displays title, track number, time, progress, volume, battery and mode.

## Music and fonts

See [the music source table](../../assets/music/README.md) for all six recordings
and original excerpt positions. The WAV clips total 165.1405 seconds. The
16 kHz mono IMA ADPCM payload occupies 1,321,124 bytes in Flash.
`assets/music/playlist.json` defines the display titles and ordering;
`assets/music/sources.json` records source pages, versions, trims and hashes.
WAV files are already trimmed, so every playlist start is 0.

```bash
python3 tools/pack_bgm.py --require-all
```

The packer requires Python 3 and FFmpeg, applies peak limiting and short fades,
and encodes 4-bit IMA ADPCM. It rejects audio above 5 MiB; the firmware gate
validates the complete 8 MB layout. Release builds require all six inputs.
The committed generated audio and font C sources allow normal firmware builds
without downloading source media or installing a font converter.

Noto Sans CJK SC subsets cover all fixed UI text and track titles at 12, 16 and
20 pixels. Page creation checks every inventoried glyph and a known missing
negative case. Title changes require LVGL font converter 1.5.3:

```bash
python3 tools/generate_bgm_fonts.py --converter <path-to-lv_font_conv>
python3 tools/pack_bgm.py --require-all
./tools/validate.sh
```

## Architecture

`main/main.c` initializes the display and buttons, creates the player and starts
its workers. Button callbacks enqueue actions without blocking. One audio
worker owns the model, codec, volume and PCM, decoding 512-sample chunks from
Flash. It uses an 8 KiB stack and one private static 1 KiB PCM buffer, also
used for the ending silence. The compiler limits its function frames to
512 bytes; playback logs its minimum remaining stack.

The UI worker consumes copied snapshots and takes the LVGL lock for short
updates. Both workers acknowledge stop before their queues, handles or screen
can be deleted; a timeout retains ownership and allows retry. Radio stacks and
hardware-test pages are not linked. The NVS/PHY/factory partition layout is
preserved. Playback does not write persistent settings.

## Validation and firmware

`./tools/validate.sh` runs repository checks, host tests, a fresh ESP-IDF build,
merged-image verification and a matching debug archive. Host tests exercise
playback state, ADPCM reference vectors, chunk boundaries, six-file packing,
worker creation failure, stop timing, retry and screen ownership.

On-device acceptance checks direct boot, Chinese rendering, playback/pause,
track switching, repeat modes, inactive double clicks, volume limits, battery
and long-duration playback. Build results and device results are reported
separately.

Flash only the verified merged `build/FoloToy-AI-Passport-full.bin` at `0x0`.
Keep its SHA-256 archive and matching ELF locally for crash decoding. Build
products and device logs belong in ignored local directories.
See [community publishing materials](queen-bgm-publishing.md) for release inputs.
