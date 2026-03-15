# SUNNY_OS `apps/` 应用开发指南

本指南面向 `SUNNY_OS/apps/` 下的应用开发，目标是让 app 能独立演进，同时与外设与平台代码（sensor/WiFi/蓝牙等）低耦合对接。

## 1. 现有框架速览

SUNNY_OS 的“应用模块接口”是 `vsun::core::AppSlice`（见 `SUNNY_OS/core/app_slice.h`）。

- 每个 app 通常实现一个 `*Slice`，负责生命周期与“懒加载 UI”（`ensure_built`）。
- `Slice` 通常组合一个 `Model`（状态/订阅/业务数据）和一个 `View`（LVGL UI）。
- 应用注册与启动入口：`vsun::core::AppController`（见 `SUNNY_OS/core/app_controller.h/.cpp`）。
- 解耦机制：`vsun::services::DataCenter` 发布/订阅 topic（见 `SUNNY_OS/services/datacenter.h`）。示例：状态栏订阅时间/电量（见 `SUNNY_OS/core/status_bar_view.cpp`）。

## 2. 单个 App 的推荐目录与命名

建议每个 app 一个目录，命名与现有保持一致：

- 目录：`SUNNY_OS/apps/<app_name>/`
- 文件：`<app_name>_slice.{h,cpp}`、`<app_name>_view.{h,cpp}`、`<app_name>_model.{h,cpp(可选)}`
- 命名空间：`namespace vsun::apps::<app_name> { ... }`

参考：`SUNNY_OS/apps/maps/`、`SUNNY_OS/apps/placeholder/`。

## 3. Slice / Model / View 职责边界

### 3.1 Slice（生命周期与装配）

`Slice` 只做三件事：

- 懒加载：`ensure_built(parent)` 第一次创建 UI、绑定数据；后续直接返回。
- 生命周期：按需实现 `on_show/on_hide`，做订阅绑定/解绑、刷新 UI、暂停动画/计时器等。
- 装配：把 `Model` 的数据“推”给 `View`（调用 `view_.set_xxx(...)`），把 `View` 的事件“回传”给 `Model/Service`。

### 3.2 View（只关心 LVGL，对外暴露 setter）

`View` 的职责是：

- `build(parent)`：创建 LVGL 对象树并保存必要的对象指针（如 `lv_obj_t* label_`）。
- 提供 `set_xxx(...)`：更新文本、颜色、可见性等。
- 尽量避免直接依赖 HAL/平台 API。

如果 `View` 需要订阅 `DataCenter`，务必处理解除订阅：

- 推荐在 `build()` 里 `subscribe(...)`，并在 root 的 `LV_EVENT_DELETE` 回调里 `unsubscribe(...)`（参考 `SUNNY_OS/core/status_bar_view.cpp`）。

### 3.3 Model（状态/订阅/缓存，尽量无 UI 依赖）

`Model` 的职责是：

- 缓存业务状态（连接状态、最新 sensor 值、扫描列表等）。
- 订阅 `DataCenter` topic 并把 payload 转成内部状态。
- 对外提供“纯数据”的 getter（或简洁字段），由 `Slice` 决定如何渲染。

建议 `Model` 不持有 `lv_obj_t*`，不直接调用 `lv_label_set_text` 等 UI API。

## 4. 新增一个 App 的最小步骤

### 4.1 新建代码文件

以 `foo` 为例，新建：

- `SUNNY_OS/apps/foo/foo_slice.h/.cpp`
- `SUNNY_OS/apps/foo/foo_view.h/.cpp`
- `SUNNY_OS/apps/foo/foo_model.h`（可选）

编译层面：`SUNNY_OS/Simulator/CMakeLists.txt` 使用 `file(GLOB_RECURSE ...)` 收集源码，一般不需要额外改 CMake。

### 4.2 注册到 Launcher / AppController

在 `SUNNY_OS/core/app_controller.h/.cpp` 做三处改动：

- `#include "apps/foo/foo_slice.h"`
- 在 `AppController` 成员里添加 `apps::foo::FooSlice foo_{};`
- 把 `&foo_` 加入 `init()` 里的 `slices_ = {...};`
- 在 `kEntries` 里添加一条入口：`{"foo", "FOO", LV_SYMBOL_...}`

注意：当前 `slices_` 和 `kEntries` 都是固定大小数组；如果增加更多 app，需要同步调整数组大小。

### 4.3 快速占位页（没有业务逻辑时）

如果 app 只是先占位一个页面，可以复用 `placeholder`：

- 继承 `vsun::apps::placeholder::PlaceholderSliceBase`
- 只提供 `id/label`

参考：`SUNNY_OS/apps/placeholder/placeholder_slice_base.h`。

## 5. 外设解耦：Sensor / WiFi / 蓝牙的推荐对接方式

核心原则：

- UI（`View`）不直接依赖 HAL/驱动。
- Slice/Model 不直接轮询硬件寄存器，也不在 UI 回调里做耗时逻辑。
- 用“服务层 + Topic 总线”解耦：服务层负责采集/状态机/命令执行，UI 通过 `DataCenter` 订阅状态变化。

### 5.1 Topic 设计（数据上行）

Topic 与 payload 建议集中定义在 `SUNNY_OS/services/datacenter_topics.h`：

- Topic 名称表达“数据含义”，而不是“来源模块”。
- payload 尽量用 POD/定长字段，利于移植到 RTOS（减少动态分配）。

项目里已预留示例 topic（`WifiState/BleState/Accel`），可在此基础上扩展。

### 5.2 Service 设计（采集与命令下发）

建议为每类外设做 service：

- `SystemStatusService`：系统状态（时间/电量等），统一发布到 DataCenter（现已落地：`SUNNY_OS/services/system_status_service.*`）。
- `ConnectivityService`：WiFi/蓝牙连接与扫描状态机（现已落地骨架：`SUNNY_OS/services/connectivity_service.*`）。
- `SensorService`：传感器采样/滤波/校准（现已落地骨架：`SUNNY_OS/services/sensor_service.*`）。

运行时由 `AppController::update()` 统一调度这些 service，发布到 `DataCenter`，由 app 订阅消费。

### 5.3 App 侧订阅与解绑

推荐把订阅放在 `Model` 或 `View`，并确保解绑：

- View 订阅：在 root 的 `LV_EVENT_DELETE` 中解绑（参考 `SUNNY_OS/core/status_bar_view.cpp`）。
- Slice/Model 订阅：在 `on_hide()` 里解绑，或在对象生命周期结束时解绑。

订阅回调建议：

- 校验 `len == sizeof(PayloadType)`（参考 `StatusBarView::on_data`）。
- 回调内只更新状态，不做重活；渲染尽量走 `Slice -> View::set_*`。

### 5.4 命令下发的解耦（按钮触发 WiFi/蓝牙操作）

推荐调用链：

- `View` 产生 UI 事件（按钮点击）。
- `Slice` 接到事件，调用 `ConnectivityService` 的命令 API（后续在 service 里补齐 `start_scan/connect/disconnect` 等）。
- 执行进度与结果仍通过 `DataCenter` topic 回来，UI 不阻塞等待。

