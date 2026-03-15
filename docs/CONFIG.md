# Sunny_OS 配置说明

## UI 布局与时序（`config/ui_config.h`）

`Sunny_OS/config/ui_config.h` 定义了 UI 尺寸/偏移/动画时序等宏。

可以在编译期通过 `-D...` 覆盖，例如：

- `-DSUNNY_UI_STATUS_BAR_H=32`
- `-DSUNNY_UI_LAUNCHER_ICON_PX=56`

常见分组：

- 模拟器分辨率：`SUNNY_UI_SIM_HOR_RES`、`SUNNY_UI_SIM_VER_RES`
- 全局栏/头：`SUNNY_UI_STATUS_BAR_H`、`SUNNY_UI_HINT_BAR_H`、`SUNNY_UI_APP_HEADER_H`
- Desktop：`SUNNY_UI_DESKTOP_*`
- Launcher：`SUNNY_UI_LAUNCHER_*`
- 开关机：`SUNNY_UI_BOOT_*`、`SUNNY_UI_SHUTDOWN_*`、`SUNNY_UI_PWR_TEXT_*`
- 转场：`SUNNY_UI_TRANSITION_*`

## LVGL 核心配置

LVGL 的内存/绘制缓冲区/色深等，由宿主工程（模拟器或目标 OS）侧的 LVGL 配置（通常是 `lv_conf.h`）控制。

