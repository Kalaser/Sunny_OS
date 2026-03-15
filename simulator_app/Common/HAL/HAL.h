#pragma once

namespace HAL {

// Simulator 下的最小 HAL 占位实现。
// SUNNY_OS 当前主要是 UI Shell，暂不依赖传感器/音频等硬件能力。
// 当移植到 MCU/RTOS 平台时，建议由平台工程提供完整 HAL（或改为 DataCenter/Service 的方式）。
void HAL_Init();
void HAL_Update();

} // namespace HAL
