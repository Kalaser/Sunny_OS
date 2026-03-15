# SUNNY_OS 命名规范

本文用于统一 `SUNNY_OS/` 目录内的命名风格，减少跨模块协作成本。

## 目录与文件

- 目录：`lower_snake_case`（例如 `apps/`、`core/`、`services/`）。
- 源码文件：`lower_snake_case.{h,cpp}`（例如 `app_controller.cpp`）。
- 应用目录：每个 app 一个目录，放在 `apps/<app_name>/`。

## C++ 命名

- 命名空间：`vsun::<layer>::...`（例如 `vsun::core`、`vsun::services`、`vsun::apps::maps`）。
- 类型（class/struct/enum）：`PascalCase`。
- 函数/方法：`lower_snake_case`（例如 `ensure_built`、`on_show`）。
- 私有成员：后缀下划线（例如 `built_`、`root_`）。
- 常量：
  - `static constexpr`：`kPascalCase`（例如 `kHeaderH`）。
  - 配置宏（仅 `config/`）：`ALL_CAPS_WITH_PREFIX`（例如 `SUNNY_UI_STATUS_BAR_H`）。

## App ID 与显示名

- `AppSlice::id()`：稳定的 `lower_snake_case`（例如 `"weather"`），用于路由/注册/持久化 key。
- `AppSlice::label()`：面向用户的显示文本（当前风格多为大写，例如 `"WEATHER"`）。

## 注释与编码

- 头文件注释建议使用简短英文，避免不同编辑器/工具链出现编码差异。
- 较长说明写进 Markdown（`SUNNY_OS/docs/` 或 `apps/APP_DEV_GUIDE.md`）。

