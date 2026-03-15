#pragma once

namespace vsun {

// SUNNY_OS 的对外入口（由宿主/平台工程调用）。
// 约定:
// - app_init(): 创建 UI Shell（Desktop/Launcher/Overlay）并完成回调绑定
// - app_update(): 可选的周期更新（由宿主以 10~20ms 调用一次较合适）
// - app_uninit(): 释放/反初始化（如有需要）
void app_init();
void app_update();
void app_uninit();

} // namespace vsun
