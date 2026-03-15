# SUNNY_OS 移植指南（FreeRTOS、NuttX、Linux、RT-Thread）

本文说明把 SUNNY_OS 从 Win32 模拟器迁移到真实 OS/硬件时，哪些模块需要替换，应该落在哪里，以及推荐的集成步骤。

## SUNNY_OS 的基本假设

- 有 LVGL tick（`lv_tick_inc`）以及定期调用 `lv_timer_handler()` 的事件循环。
- 已实现 LVGL 显示驱动（`flush_cb`）与输入驱动（indev：touch/encoder/key 等）。
- 需要时实现 LVGL 文件系统驱动（用于加载外部资源）。
- 平台钩子（SUNNY_OS 内部会调用）：
  - 时间：`vsun::port::get_local_time_hhmm(...)`（`SUNNY_OS/port/time.h`）
  - 关机/退出：`vsun::port::request_shutdown()`（`SUNNY_OS/port/system.h`）
  - 设备状态：`vsun::hal::*`（`SUNNY_OS/sunny_hal/hal.h`）

当前模拟器使用：

- `SUNNY_OS/port/time_std.cpp`
- `SUNNY_OS/port/system_win32.cpp`
- `SUNNY_OS/sunny_hal/hal_sim.cpp`

## 通用建议（强烈推荐）

1. 所有 LVGL API 只在一个 UI 线程/任务中调用（避免线程安全问题）。
2. 硬件采集与状态机放在 service/HAL 中，通过 `services::DataCenter` 发布状态。
3. UI（slice/view）订阅 topic 并更新展示，不要阻塞等待硬件命令。

## FreeRTOS（典型 MCU）

- Tick：硬件定时器 ISR 每 1ms 调 `lv_tick_inc(1)`（或软件定时器）。
- UI 任务：每 10-20ms 调 `lv_timer_handler()` 与 `vsun::app_update()`。
- 显示/输入：实现 LVGL 驱动（SPI/8080/DMA、touch/encoder/buttons 等）。
- 存储：把持久化映射到 NVS/FlashDB/LittleFS 等，尽量保持 `StorageService` 的 API 不变。
- 文件落点建议：
  - `SUNNY_OS/port/time_rtc.cpp`
  - `SUNNY_OS/port/system_freertos.cpp`
  - `SUNNY_OS/sunny_hal/hal_freertos.cpp`（或更贴近板级命名）

## NuttX

- 时间/系统：优先用 POSIX-like API（`clock_gettime`、RTC），电源/重启按平台可用接口实现（如 `boardctl`）。
- 显示/输入：用 NuttX 图形/输入子系统承载 LVGL 显示与 indev。
- 存储：使用 NuttX FS 或 KV 后端，保持 `services` 接口稳定。

## Linux

- Tick/循环：timerfd/SDL 等方式提供 tick；主循环调用 `lv_timer_handler()`。
- 显示/输入：SDL、DRM/KMS、fbdev、Wayland 等按平台选择。
- 时间：在 `port/time_linux.cpp` 中用 `localtime_r` 等实现 `get_local_time_hhmm`。
- 关机/退出：在 `port/system_linux.cpp` 对接 systemd/DBus 或平台钩子（也可先做“通知上层退出”）。
- 存储：将 KV 文件放在 `/var/lib/...` 或应用数据目录。

## RT-Thread

- LVGL：使用 RT-Thread LVGL port/package，确保 tick 与 handler 线正确。
- 并发：保持 LVGL 单线程；工作线程通过 DataCenter 或消息队列把数据发到 UI 任务。
- 时间/系统：`vsun::port` 对接 RT-Thread RTC 与电源管理接口。
- 存储：EasyFlash KVDB 或文件系统 KV，保持 `StorageService` API 稳定。

