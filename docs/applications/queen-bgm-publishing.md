<p align="right"><a href="queen-bgm-publishing.zh_CN.md">简体中文</a> · <strong>English</strong></p>

# Queen Reborn BGM publishing checklist

The [official repository guidance](../development/release/publish-to-community.md)
requires these community submission materials:

| Material | Purpose |
| --- | --- |
| Verified merged firmware | `build/FoloToy-AI-Passport-full.bin`, programmed at 0x0; embeds code, fonts and six clips |
| Cover | Representative PNG/JPEG/WebP, at most 10 MiB |
| Bilingual title and description | Name, controls, direct boot, excerpt lengths and offline behavior |
| Public HTTPS source repository | Your modified source/fork, rather than the unchanged upstream page |
| Creator account | Creator signs in on the official community; upload requires explicit approval |

Keep `main/`, `components/bsp/`, CMake/config/partitions, dependency lock,
required font assets/license, the six WAV files plus playlist, and build tools
for reproducible source. Include the existing source license and applicable
audio permissions. Generated packed audio is included in the firmware: device
users need no separate WAV import.

Do not upload local `build/` trees, ELF/map/debug logs, device readbacks,
credentials, `.git/`, `.agents/`, `.codex/`, local ESP-IDF/toolchain downloads,
audio candidates or full reference downloads as community materials. Keep the
matching ELF/manifest and private logs locally for debugging. A source ZIP is
optional and does not replace the required public HTTPS repository.

Community releases provide a cover and a public HTTPS URL for the current
source. Redistribution permission for all six recordings is unverified and
applies to both source audio and embedded firmware. See the
[music sources](../../assets/music/README.md) for recordings and excerpts.
The firmware boots directly into the player.
