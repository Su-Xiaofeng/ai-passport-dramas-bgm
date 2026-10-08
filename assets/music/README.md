<p align="right"><a href="README.zh_CN.md">简体中文</a> · <strong>English</strong></p>

# Music sources

The six offline excerpts below are the inputs used by the embedded player.
All WAV files are 16 kHz, 16-bit mono. Source links identify the actual downloaded
music-video recordings; they do not identify or license an original master.

| File | Track | Download source | Original start | Excerpt duration |
| --- | --- | --- | --- | --- |
| `01.wav` | Cage | [Bilibili P1](https://www.bilibili.com/video/BV1YMb56NEHW?p=1) | 0:00 | 30 s |
| `02.wav` | Toji Fushiguro March | [Bilibili P11](https://www.bilibili.com/video/BV1YMb56NEHW?p=11) | 0:00 | 30 s |
| `03.wav` | Villain Rescue | [Bilibili P12](https://www.bilibili.com/video/BV1YMb56NEHW?p=12) | 0:02.5 | 18.63 s |
| `04.wav` | Try | [Bilibili P8](https://www.bilibili.com/video/BV1YMb56NEHW?p=8) | 0:00 | 30 s |
| `05.wav` | Right (Bite March) | [Bilibili P6](https://www.bilibili.com/video/BV1YMb56NEHW?p=6) | 0:00 | 30 s |
| `06.wav` | Amazon | [Bilibili P2](https://www.bilibili.com/video/BV1YMb56NEHW?p=2) | 0:00 | 26.0105 s |

All six tracks use the selected Bilibili collection. Part 11 is titled
Illusory Day 4.0 (Toji Fushiguro March); Part 12 is titled Results Screen
(Villain Rescue). Try uses Part 8 and Right (Bite March) uses Part 6.
Total playback material is 164.6405 seconds.

`sources.json` contains the source-page titles, original-file SHA-256 hashes,
WAV hashes and trim positions. `playlist.json` specifies display titles, file
order and playback clips. The WAV files already contain these excerpts:
all playlist starts are 0, so no further leading trim is required.

`reference/` stores optional local source downloads and is ignored by Git.
The six WAV files, playlist, provenance JSON and generated
`main/bgm_tracks_data.c` / `.json` are repository build inputs.

```bash
python3 tools/pack_bgm.py --require-all
```

The packer uses FFmpeg and Python 3 to apply peak limiting and short fades,
then produces 4-bit IMA ADPCM for 16-bit playback. The firmware embeds
1,317,124 audio bytes and needs no separate audio import or network connection.
Editing a title also requires regenerating the CJK font subsets.

Audio redistribution permission is unverified. The repository's code license
does not grant a license for these recordings; confirm permission before
publishing audio-containing source or firmware.
