<p align="right"><a href="README.zh_CN.md">简体中文</a> · <strong>English</strong></p>

# Music sources

The six offline excerpts below are the inputs used by the embedded player.
All WAV files are 16 kHz, 16-bit mono. Source links identify the actual downloaded
music-video recordings; they do not identify or license an original master.

| File | Track | Download source | Original start | Excerpt duration |
| --- | --- | --- | --- | --- |
| `01.wav` | Cage | [Bilibili](https://www.bilibili.com/video/BV1fojt6RE7L/) | 0:00 | 30 s |
| `02.wav` | Toji Fushiguro March | [Douyin](https://www.douyin.com/video/7609996387622718771) | 0:00 | 30 s |
| `03.wav` | Villain Rescue | [Douyin](https://www.douyin.com/video/7618536846884379618) | 0:02 | 19.13 s |
| `04.wav` | Try | [Bilibili](https://www.bilibili.com/video/BV1MaqABUEAA/) | 0:00 | 30 s |
| `05.wav` | Right (Bite March) | [Bilibili](https://www.bilibili.com/video/BV1YMb56NEHW?p=6) | 0:00 | 30 s |
| `06.wav` | Amazon | [Bilibili](https://www.bilibili.com/video/BV1arHh6wEUN/) | 0:04 | 26.0105 s |

Track 5 uses Part 6 of the linked Bilibili collection, titled Right (Bite March).
Try uses the Andre Juss video recording. Its match to an electronic DJ Remix
is unverified. Total playback material is 165.1405 seconds.

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
1,321,124 audio bytes and needs no separate audio import or network connection.
Editing a title also requires regenerating the CJK font subsets.

Audio redistribution permission is unverified. The repository's code license
does not grant a license for these recordings; confirm permission before
publishing audio-containing source or firmware.
