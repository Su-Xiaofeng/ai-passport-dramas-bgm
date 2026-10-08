<p align="right">
  <strong>简体中文</strong> · <a href="README.md">English</a>
</p>

# 资源目录（Assets）

本目录集中存放可复用的资源（字库、图片、音乐等），按资源类型分子目录管理。每个资源放在其类型对应的子目录，并记录放置路径、命名方式、集成方式与来源/许可。二进制资源（字体、图片、音频）不属于纯 markdown 文档，请勿与文档混放。涉及版权/授权的资源需注明来源与许可。

## 字库（fonts）

可复用的字库文件与生成的字库源码放在 `fonts/`。

- 命名要能反映字族、字重、字级与格式。
- 记录来源、许可、字符范围、转换命令与目标放置路径。
- 添加字库前评估 Flash 与内部 RAM 影响；ESP32-C3 无 PSRAM。
- 不提交许可不允许分发的字库。

## 图片（images）

可复用的源图与生成的显示资产放在 `images/`。

| 文件 | 尺寸与格式 | 用途与来源 |
| --- | --- | --- |
| [`images/home.jpg`](images/home.jpg) | 3840 × 2160，JPEG | 嵌入中英文硬件总览 `docs/README` 的产品主图，突出 AI Passport 产品形象与开放、人人可创作的理念。 |
| [`images/readme-hardware-specs.png`](images/readme-hardware-specs.png) | 2172 × 724，PNG RGBA | 可选硬件技术参考图。于 2026-09-17 使用内置图像生成工具为本仓库生成；已根据文档中的硬件能力契约核对图中的六项标签与参数。 |
| [`images/logo-wordmark.png`](images/logo-wordmark.png) | 1648 × 336，PNG RGBA | 从仓库原始 `images/logo.png` 中精确裁切并去除背景的黑色字标；用于中英文硬件总览的浅色主题。 |
| [`images/logo-wordmark-dark.png`](images/logo-wordmark-dark.png) | 1648 × 336，PNG RGBA | 提取字标的白色版本；硬件总览使用 `<picture>` 在 GitHub 深色主题下显示。 |

- 使用描述性命名，并记录尺寸、像素格式、转换步骤与目标路径。
- 优先采用适合 240 × 320 RGB565 显示的格式，并纳入 Flash 与内部 RAM 考量。
- 许可允许时保留可编辑源文件，并记录来源与许可。
- 图片中不得包含设备二维码秘密、凭证或个人数据。

## 音乐与音效（music）

可复用的音乐与音效源码放在 `music/`。

- 记录来源、许可、采样率、位深、声道、转换命令与目标路径。
- 与当前 BSP 音频路径匹配时优先采用 16 kHz、16 位单声道 PCM。
- 嵌入音频前评估 Flash 与内部 RAM 成本；长录音应流式或分块。
- 无再分发许可不提交媒体文件。

## 重生之我是女王 BGM 资源

- `music/playlist.json`：六首当前显示歌名与播放顺序；`01.wav` 到 `06.wav`
  已包含导入的片段。来源、截取位置与实际 WAV 哈希在 `music/sources.json`。
  下载原文件由 Git 忽略，仅供本地追溯；音源与时长见 [音乐来源](music/README.zh_CN.md)。
  音频再分发授权尚未确认。
- `main/bgm_tracks_data.c` 及 JSON 报告：通过
  `python3 tools/pack_bgm.py --require-all` 生成。16 kHz 单声道，4 位 IMA ADPCM
  分块解码为 16 位 PCM，每块 512 采样。
- `fonts/NotoSansCJKsc-Regular.otf`：来自官方
  [Noto CJK 仓库](https://github.com/notofonts/noto-cjk/blob/main/Sans/OTF/SimplifiedChinese/NotoSansCJKsc-Regular.otf)，
  SIL Open Font License 1.1 许可保存在 `fonts/OFL.txt`。
- `fonts/bgm_font_12.c`、`bgm_font_16.c`、`bgm_font_20.c`：由
  LVGL `lv_font_conv` 1.5.3 生成的 4-bpp 未压缩子集。字符清单在
  `fonts/bgm_chars.txt`，编码点清单在 `fonts/bgm_glyphs.h`。
  重建命令为 `python3 tools/generate_bgm_fonts.py --converter <lv_font_conv>`。
  应用显式使用这些字库，逐字符检查并检查已知缺字，中文显示验收需检查各控件与曲名。
