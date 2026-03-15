#include "sunny_hal/hal.h"

#include <cstdint>

namespace vsun::hal {
namespace {

std::uint8_t g_batt = 95;
std::uint32_t g_last_step_ms = 0;

} // namespace

void init()
{
    g_batt = 95;
    g_last_step_ms = 0;
}

void update(std::uint32_t now_ms)
{
    // Simulator: slowly changing battery percent for UI integration.
    if(g_last_step_ms == 0) g_last_step_ms = now_ms;
    if(now_ms - g_last_step_ms < 4000) return;
    g_last_step_ms = now_ms;

    if(g_batt > 5) g_batt--;
    else g_batt = 95;
}

std::uint8_t battery_percent()
{
    return g_batt;
}

} // namespace vsun::hal

