#pragma once

#include <cstdint>

// HAL: hardware abstraction layer (Windows Simulator mock).
// On RTOS/MCU, replace this implementation with real drivers/services.
namespace vsun::hal {

void init();
void update(std::uint32_t now_ms);

// 0..100
std::uint8_t battery_percent();

} // namespace vsun::hal

