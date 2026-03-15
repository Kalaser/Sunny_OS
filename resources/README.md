# SUNNY_OS 字库/图库资源机制（Bitmap Pipeline）

目标：

- 导入图片或字库表（Excel 导出 CSV）后，编译时自动生成：
  - `resources/generated/sunny_resources.c`：资源数据与 `lv_img_dsc_t`/`lv_font_t` 对象
  - `resources/generated/sunny_resources.h`：每张图片/每套字体/每条字符串对应的宏与 `extern` 声明
  - `resources/generated/bin/*.bin`：可选的二进制资源（便于后续打包烧录/外部加载）
- 业务代码只需要 `#include "resources/generated/sunny_resources.h"`，即可通过宏拿到图片、字体、字符串。

## 1. 输入目录

- 图片：在 `SUNNY_OS/resources/manifest.json` 里通过 `images`（单文件）或 `image_dirs`（目录扫描）声明。
  - 支持 `*.bmp`（24/32bpp，BI_RGB 或 BI_BITFIELDS）
  - 支持 `*.svg`（需要本机安装 ImageMagick，并确保 `magick` 在 PATH 中）
- 字体（bitmap）：`SUNNY_OS/resources/input/fonts/`
  - 一个字体由两部分组成：
    1) 字形图集：`<font_name>.bmp`
    2) 字形表（Excel 导出 CSV）：`<font_name>.csv`

## 2. Manifest（资源清单）

编辑 `SUNNY_OS/resources/manifest.json`，声明要生成哪些资源。

- `images`：单个图片条目（bmp/svg）
- `image_dirs`：目录扫描（适合一次导入一批图标，例如 `resources/input/mynaui/*.svg`）

## 3. 字库 CSV 格式（Excel 导出）

每行一个 glyph，支持列（必须列）：

- `codepoint`：Unicode 码点，支持 `0x4E2D` 或十进制（例如 `20013`）
- `x,y,w,h`：glyph 在图集 bmp 中的矩形
- `adv_w`：字宽（advance width）
- `ofs_x,ofs_y`：相对基线偏移

额外建议列（可选）：

- `comment`：备注，不参与生成

注意：

- 图集 bmp 建议“黑底白字”（或透明底白字），生成器会用像素亮度作为 alpha（8bpp 字形）。

## 4. 代码使用

```cpp
#include "sunny_resources.h"

// 图片（lv_img_set_src 支持 lv_img_dsc_t*）
lv_img_set_src(img, SUNNY_IMG(LOGO));        // 通用宏
lv_img_set_src(img, SUNNY_IMG_LOGO);         // 每张图对应的独立宏（生成器会自动生成）

// 字体（lv_obj_set_style_text_font 需要 lv_font_t*）
lv_obj_set_style_text_font(label, SUNNY_FONT(MYFONT_16), 0);   // 通用宏
lv_obj_set_style_text_font(label, SUNNY_FONT_MYFONT_16, 0);    // 独立宏

// 字符串
lv_label_set_text(label, SUNNY_STR(APP_TITLE)); // 通用宏
lv_label_set_text(label, SUNNY_STR_APP_TITLE);  // 独立宏
```

## 5. 构建集成

`SUNNY_OS/Simulator/CMakeLists.txt` 已接入生成步骤：每次 build 会先运行 `tools/assets/gen_assets.py`，生成 `resources/generated/sunny_resources.c/.h` 后再编译。

如果你新增/删除了资源文件，建议重新运行一次 CMake 配置（或直接删掉 `Simulator/build` 重新 build），保证资源文件列表刷新。
