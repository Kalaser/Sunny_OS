#pragma once

#include <cstdint>

namespace vsun {

struct CarouselItemLayout {
    int16_t x;       // relative to center
    int16_t y;       // relative to center (down is positive)
    uint16_t size;   // icon circle size (px)
    uint8_t opa;     // 0..255
    bool visible;
};

CarouselItemLayout compute_carousel_item_layout(
    int index,
    int selected_index,
    int radius_px,
    int angle_step_deg
);

} // namespace vsun

