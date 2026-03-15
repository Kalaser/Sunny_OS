# SUNNY_OS 架构说明

SUNNY_OS 是一套基于 LVGL 的应用层：提供轻量 UI Shell（Desktop/Launcher/App chrome）、页面路由（`services::PageManager`）以及发布订阅总线（`services::DataCenter`）。

## 分层与职责

- `runtime/`：对宿主暴露的入口（`vsun::app_init/app_update/app_uninit`）。
- `core/`：UI Shell 与应用注册（`AppController`、主题、状态栏等）。
- `services/`：非 UI 的基础设施与服务（`PageManager`、`DataCenter`、`StorageService`、各类 service）。
- `apps/`：应用模块（实现 `vsun::core::AppSlice`）。
- `desktop/`、`launcher/`：Shell 自带的 slice（由 core 组装成页面）。
- `pages/`：整页页面（例如 boot/shutdown），由 `services::PageManager` push/pop/replace。
- `sunny_hal/`：硬件抽象层（模拟器 mock 或真实板级实现）。
- `port/`：OS/平台差异层（时间、关机/退出、等）。
- `config/`：编译期配置宏（UI 尺寸、动画时序等）。

## UI 推荐模式：Slice / Model / View

每个 app（`apps/<name>/`）推荐拆成：

- `*Slice`：生命周期与懒加载（`ensure_built`），把 model 和 view 组装起来。
- `*Model`：纯状态与订阅回调，避免持有 LVGL 对象指针。
- `*View`：只做 LVGL 对象树与 `set_*` 更新接口，避免直接依赖 HAL/平台 API。

## 解耦机制：`services::DataCenter`

- 生产者（HAL/service）发布 `Topic + payload`。
- 消费者（model/view/slice）订阅 topic 并更新状态/UI。
- 必须在对象销毁/页面隐藏时解除订阅，避免回调访问失效 UI 指针。

## 并发建议：UI 线程独占 LVGL 与 DataCenter

推荐线程模型：

- UI 线程独占：所有 LVGL API 与 `services::DataCenter::{publish,subscribe,unsubscribe}` 都只在 UI 线程调用。
- Worker 线程只做采集/协议栈/状态机，不直接碰 UI 指针。
- 跨线程上报：worker 线程把 `Topic + payload` 入队，UI 线程每帧/tick drain 并统一发布到 DataCenter。

SUNNY_OS 已提供一个最小实现：`services::UiPublishQueue`（见 `SUNNY_OS/services/ui_publish_queue.*`），UI 侧建议在 `AppController::update()` 开头调用 `UiPublishQueue::drain()`。

`services::ConnectivityService` 也提供了跨线程/跨平台的上报接口：

- `report_wifi_state(LinkState state, int8_t rssi_dbm)`
- `report_ble_state(LinkState state)`

建议平台侧驱动/协议栈回调在任意线程调用这些方法，上报会通过 `UiPublishQueue` 安全地在 UI 线程发布到 `DataCenter`。

## （参考）
- SUNNY_OS：`sunny_hal/port` + `services::DataCenter` + `services::PageManager` + `AppSlice`。
