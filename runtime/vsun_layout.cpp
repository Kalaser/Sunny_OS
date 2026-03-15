#include "vsun_layout.h"

#include <cmath>

#include "config/ui_config.h"

namespace vsun {

static int clamp_int(int v, int lo, int hi)
{
    if(v < lo) return lo;
    if(v > hi) return hi;
    return v;
}

CarouselItemLayout compute_carousel_item_layout(
    int index,
    int selected_index,
    int radius_px,
    int angle_step_deg)
{
    CarouselItemLayout out{};

    const int delta = index - selected_index;
    const int abs_delta = (delta < 0) ? -delta : delta;

    // Keep the list bounded so callers don't have to special-case out-of-range indices.
    // Limit to +/-3 to avoid the farthest items collapsing to the bottom center (e.g. 180 degrees).
    if(abs_delta > 3 || radius_px <= 0 || angle_step_deg <= 0) {
        out.visible = false;
        out.x = 0;
        out.y = 0;
        out.size = 0;
        out.opa = 0;
        return out;
    }

    const double rad = (double)(delta * angle_step_deg) * 3.14159265358979323846 / 180.0;

    // Layout items on a shallow arc around the center.
    out.x = (int16_t)std::lround(std::sin(rad) * (double)radius_px);
    out.y = (int16_t)std::lround((1.0 - std::cos(rad)) * (double)radius_px * 0.32);

    // Use an absolute icon size so the view can scale relative to its base size.
    // Keep in sync with launcher base icon size; override via config/ui_config.h.
    const int base_px = SUNNY_UI_LAUNCHER_ICON_PX;
    const int min_px = 36;
    out.size = (uint16_t)clamp_int(base_px - abs_delta * 8, min_px, base_px);

    out.opa = (uint8_t)clamp_int(255 - abs_delta * 42, 0, 255);
    out.visible = true;
    return out;
}

} // namespace vsun
