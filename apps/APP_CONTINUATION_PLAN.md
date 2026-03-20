# SUNNY_OS `apps/` 延续开发文档

本文档用于承接当前 `apps/` 页面建设状态，明确下一阶段的实现边界、优先级与落地步骤。

## 1. 当前完成情况（2026-03）

已具备可展示页面的应用如下：

- 完整独立页面：`maps`、`ai`、`camera`
- 基于通用占位页面：`translate`、`weather`、`music`、`notes`、`health`、`radio`、`settings`

其中占位页已统一接入 `PlaceholderSliceBase + PlaceholderView`，每个 app 均有独立标题、图标、副标题、详情、标签与进度内容，可用于联调导航与视觉走查。

## 2. 代码结构约定（延续）

保持并延续现有架构：

- `*Slice`：生命周期、页面装配、订阅时机
- `*Model`：数据状态缓存与 topic 订阅（无 UI 依赖）
- `*View`：LVGL 视图树与 setter

新增功能优先在现有目录内演进，不建议跨 app 复制 UI 代码。

## 3. 通用占位页到业务页的替换流程

当某个 app 从占位页升级为业务页时，按以下步骤执行：

1. 新建 `<app>/<app>_view.{h,cpp}` 与 `<app>/<app>_model.{h,cpp(可选)}`。
2. 将 `<app>_slice.h` 从继承 `PlaceholderSliceBase` 改为继承 `core::AppSlice`。
3. 在 `ensure_built()` 中调用 `view_.build(parent)`，并绑定 model 数据到 view。
4. 如有订阅，按生命周期在 `on_show/on_hide` 中做绑定和解绑。
5. 保持 `id()` 不变，避免 launcher 路由失效。

## 4. 每个 app 的下一步建议

### 4.1 Translate
- 增加输入语言 / 输出语言切换控件
- 增加最近翻译历史列表（本地缓存）
- 预留语音输入状态展示

### 4.2 Weather
- 增加小时级预报（未来 12 小时）
- 增加空气质量详情（PM2.5、PM10）
- 增加城市选择与缓存策略

### 4.3 Music
- 增加播放控制（上一首/播放暂停/下一首）
- 增加进度条与时长显示
- 对接音量与输出设备状态

### 4.4 Notes
- 增加笔记列表与置顶能力
- 增加编辑态（软键盘/输入法适配）
- 增加本地持久化与冲突合并策略

### 4.5 Health
- 增加步数、心率、睡眠三卡片详情
- 增加日/周趋势简图
- 对接 `SensorService` 订阅节流更新

### 4.6 Radio
- 增加频率调谐、收藏电台
- 增加信号强度动态刷新
- 增加连接中/重连失败状态机

### 4.7 Settings
- 增加设置分组（网络、显示、声音、系统）
- 增加二级页面跳转范式
- 对接 `StorageService` 保存用户配置

## 5. 与服务层对接建议

推荐统一通过 `DataCenter` topic 对接，不在 view 里直接访问 HAL：

- 网络相关：`ConnectivityService`
- 传感器相关：`SensorService`
- 状态栏/系统态：`SystemStatusService`

可先完成“可回放 mock 数据”链路，再替换为真实驱动输入。

## 6. 迭代顺序建议

按交付价值与依赖关系，建议顺序：

1. `Settings`（基础配置）
2. `Weather`（信息展示强）
3. `Music`（交互闭环清晰）
4. `Notes`（输入与存储）
5. `Health`（依赖 sensor）
6. `Translate`（语音/文本双通道）
7. `Radio`（依赖连接稳定性）

## 7. 联调与验收清单

- 页面能从 launcher 正确进入并返回
- 首次进入与重复进入不崩溃（懒加载正确）
- 横竖屏/窗口变化时布局可重算
- 不出现未解绑订阅导致的重复回调
- 无阻塞 UI 的长耗时逻辑

---

维护说明：当某个 app 占位页被替换为真实业务页时，请同步更新“第 1 节 当前完成情况”和“第 4 节 下一步建议”。
