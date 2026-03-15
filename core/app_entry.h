#pragma once

#include "lvgl.h"

namespace vsun::core {

struct AppEntry {
    const char* id;
    const char* label;
    const lv_img_dsc_t* icon_img;
};

} // namespace vsun::core
