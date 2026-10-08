<p align="right"><strong>简体中文</strong> · <a href="README.md">English</a></p>

# 音乐来源

以下六段离线音频是播放器实际使用的输入，WAV 均为 16 kHz、16 位单声道。
链接标明实际下载的音乐视频版本，不代表原始母带来源或音频授权。

| 文件 | 曲名 | 下载来源 | 原音频起点 | 片段时长 |
| --- | --- | --- | --- | --- |
| `01.wav` | 囚笼 | [B站](https://www.bilibili.com/video/BV1fojt6RE7L/) | 0:00 | 30 秒 |
| `02.wav` | 伏黑甚尔进行曲 | [抖音](https://www.douyin.com/video/7609996387622718771) | 0:00 | 30 秒 |
| `03.wav` | 反派烧气救场 | [抖音](https://www.douyin.com/video/7618536846884379618) | 0:02 | 19.13 秒 |
| `04.wav` | Try | [B站](https://www.bilibili.com/video/BV1MaqABUEAA/) | 0:00 | 30 秒 |
| `05.wav` | Right（撕咬进行曲） | [B站](https://www.bilibili.com/video/BV1YMb56NEHW?p=6) | 0:00 | 30 秒 |
| `06.wav` | Amazon | [B站](https://www.bilibili.com/video/BV1arHh6wEUN/) | 0:04 | 26.0105 秒 |

第五首使用链接合集的第 6 分 P《Right（撕咬进行曲）》。
Try 使用 Andre Juss 的视频音源，是否对应电子 DJ Remix 尚未确认。
六段音频共 165.1405 秒。

`sources.json` 记录来源页面标题、下载原文件 SHA-256、WAV 哈希和原始截取位置。
`playlist.json` 定义显示歌名、播放顺序与片段。WAV 已包含这些片段，配置中的
start 均为 0，无需再次裁剪开头。

`reference/` 保存可选的本地下载原文件，Git 忽略该目录。六个 WAV、歌单、来源
JSON 和生成的 `main/bgm_tracks_data.c` / `.json` 是仓库构建输入。

```bash
python3 tools/pack_bgm.py --require-all
```

打包使用 FFmpeg 与 Python 3，限峰、添加短淡入淡出后编码为 4 位 IMA ADPCM，
播放时解码为 16 位 PCM。固件内置 1,321,124 字节音频，不需要另行导入或联网。
修改显示歌名时还需重新生成中文子集字体。

音频再分发授权尚未确认。仓库的代码许可不包含这些录音的授权，公开发布含音频
的源码或固件前需确认许可。
