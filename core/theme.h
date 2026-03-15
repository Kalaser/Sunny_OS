#pragma once

#include "lvgl.h"

namespace vsun::core::theme {

static inline lv_color_t primary() { return lv_color_hex(0x00FF66); }
static inline lv_color_t secondary() { return lv_color_hex(0x7CFFB2); }
static inline lv_color_t divider() { return lv_color_hex(0x003322); }
static inline lv_color_t hint() { return lv_color_hex(0x3BAF6F); }

static inline void apply_screen_base(lv_obj_t* scr)
{
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
}

} // namespace vsun::core::theme
