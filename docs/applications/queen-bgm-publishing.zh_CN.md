<p align="right"><strong>简体中文</strong> · <a href="queen-bgm-publishing.md">English</a></p>

# 重生之我是女王 BGM：社区发布清单

依据[原仓库官方发布说明](../development/release/publish-to-community.zh_CN.md)，
提交社区需要以下材料：

| 材料 | 用途 |
| --- | --- |
| 验证通过的合并固件 | `build/FoloToy-AI-Passport-full.bin`，地址 0x0；已包含代码、字体、六段音频 |
| 封面 | 有代表性的 PNG/JPEG/WebP，最大 10 MiB |
| 中英文标题和介绍 | 项目名、按键、开机直达、片段时长与离线播放方式 |
| 公开 HTTPS 源码仓库 | 包含修改后的源码/fork，不能只填未修改的上游仓库地址 |
| 创作者账号 | 用户本人在官方社区登录；正式上传需要明确授权 |

可复现源码要保留 `main/`、`components/bsp/`、CMake/配置/分区、依赖锁、所需字体
及许可、六个 WAV 和歌单、构建工具，以及原代码许可和适用的音频授权。
音频已打包在固件内，设备使用者不需要另外导入 WAV。

社区材料不需要上传本地 `build/` 目录、ELF/map/调试日志、设备读回文件、凭据、
`.git/`、`.agents/`、`.codex/`、本地 ESP-IDF/工具链下载、候选音频或完整来源下载。
匹配固件的 ELF/manifest 与私人日志留在本地用于排障。源码 ZIP 可选，不能替代
社区要求的公开 HTTPS 源码仓库。

社区发布应提供封面与当前源码的公开 HTTPS URL。六首录音的公开再分发授权
尚未确认，源码音频和内置音频固件都需确认许可。音源与截取位置见
[音乐来源](../../assets/music/README.zh_CN.md)。固件直接开机进入播放器。
