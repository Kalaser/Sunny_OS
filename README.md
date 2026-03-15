# SUNNY_OS (LVGL v8 Win32 Simulator)

SUNNY_OS 是一套基于 LVGL 的应用层代码。它不提供自己的 `main()`；由宿主（Win32 模拟器或目标 OS）通过以下入口驱动：

- `vsun::app_init()`
- `vsun::app_update()`
- `vsun::app_uninit()`

本目录包含 Win32 模拟器工程：`SUNNY_OS/Simulator/`（CMake + MinGW 或 MSVC）。

说明：

- Win32 模拟器的 `main()` 入口在 `Sunny_OS/Simulator/host_win32/sunny_os_host_win32.cpp`（SUNNY_OS 自己的宿主程序）。
- 目前仅 LVGL 源码从 `Sunny_OS/libs/lvgl/` 目录以源码方式编译进可执行文件；`win32drv` 与 `lv_fs_if` 已迁入 `SUNNY_OS/third_party/` 由 SUNNY_OS 自己编译维护。

## 构建（Win32 模拟器）

前置条件：

- CMake
- TDM-GCC / MinGW（`PATH` 中能找到 `gcc/g++/mingw32-make`）

构建：

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

等价命令：

```powershell
cmake -S .\Simulator -B .\Simulator\build -G "MinGW Makefiles"
cmake --build .\Simulator\build -j 10
```

如果你的环境里有 `make`（例如 MSYS2/MinGW），也可以用 `SUNNY_OS/Makefile`：

```powershell
make -C . build
make -C . run
```

## 运行

前台运行：

```powershell
powershell -ExecutionPolicy Bypass -File .\run.ps1
```

后台启动：

```powershell
powershell -ExecutionPolicy Bypass -File .\run.ps1 -Detach
```

## 文档

- ### 应用开发

  - [APP_DEV_GUIDE.md](Sunny_OS/apps/APP_DEV_GUIDE.md) 
      `apps/` 目录应用开发指南，包括 Slice / Model / View 架构以及外设解耦建议。

  ### 系统设计

  - [ARCHITECTURE.md](Sunny_OS/docs/ARCHITECTURE.md) 
      Sunny_OS 整体架构与系统分层说明。

  - [NAMING.md](Sunny_OS/docs/NAMING.md) 
      项目命名规范与代码风格。

  - [CONFIG.md](Sunny_OS/docs/CONFIG.md) 
      系统配置说明，例如 `ui_config.h`、UI 宏配置等。

  ### 移植指南

  - [PORTING_GUIDE.md](Sunny_OS/docs/PORTING_GUIDE.md) 
      移植到 **FreeRTOS / NuttX / Linux / RT-Thread** 的关键接口与步骤。

## 备注

- Win32 模拟器默认会在工作目录生成 `sunny_os.kv`（由 `services/StorageService` 用于 KV 持久化）。
