<p align="right"><strong>简体中文</strong> · <a href="README.md">English</a></p>

# 重生之我是女王 BGM

项目源码： [Su-Xiaofeng/ai-passport-dramas-bgm](https://github.com/Su-Xiaofeng/ai-passport-dramas-bgm/tree/feature/queen-bgm)。

面向 FoloToy AI Passport 的离线六曲 BGM 播放器，基于
[FoloToy/ai-passport](https://github.com/FoloToy/ai-passport)。
开机直接进入黑金色 240 x 320 播放页，默认暂停、音量 45%、单曲循环。
界面显示歌名、进度、音量、电量与播放模式；音乐内置在固件中。

## 按键

| 按键 | 短按 | 长按 | 双击 |
| --- | --- | --- | --- |
| 上 | 上一首 | 音量 +10 | 无操作 |
| 下 | 下一首 | 音量 -10 | 无操作 |
| OK | 播放/暂停 | 切换单曲循环/播放一次 | 无操作 |

切歌首尾环绕，暂停保留进度，不提供 demo 菜单。
项目标题使用 16 像素字体，长按说明显示在同一排。

## 构建与烧录

目标为 ESP32-C3、8 MB Flash、无 PSRAM、ESP-IDF 5.5.3。
激活 ESP-IDF 环境，并安装音频测试与打包需要的 FFmpeg。

```bash
python3 tools/pack_bgm.py --require-all
./tools/validate.sh
```

使用验证通过的合并镜像 `build/FoloToy-AI-Passport-full.bin`，烧录地址为 `0x0`。
仓库包含六个 WAV 片段及生成的音频/字体源码，可复现构建；构建产物、本地下载
原文件和调试日志由 Git 忽略。

`feature/queen-bgm` 分支用于本播放器；仓库 `main` 保留上游硬件测试基线和
项目目录。构建本玩法时，克隆播放器分支：

```bash
git clone --branch feature/queen-bgm https://github.com/Su-Xiaofeng/ai-passport-dramas-bgm.git
```

## 文档

- [播放器结构、界面与验证](docs/assets/queen-bgm.zh_CN.md)
- [六首音乐来源与片段时长](assets/music/README.zh_CN.md)
- [社区发布材料](docs/assets/queen-bgm-publishing.zh_CN.md)
- [硬件与开发指南](docs/README.zh_CN.md)

## 许可

代码采用 [MIT 许可](LICENSE)，Noto Sans CJK 字体采用
[SIL Open Font License 1.1](assets/fonts/OFL.txt)。
音乐再分发授权尚未确认，代码许可不包含所附录音的授权。
