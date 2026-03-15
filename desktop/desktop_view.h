#pragma once

#include <array>
#include <cstdint>

#include "config/ui_config.h"
#include "lvgl.h"

#include "core/status_bar_view.h"

namespace vsun::desktop {

struct DesktopParticle {
    lv_obj_t* obj = nullptr;
    lv_coord_t r = 0;
    // fixed-point (8.8) center coordinates
    std::int32_t cx = 0;
    std::int32_t cy = 0;
    std::int32_t vx = 0;
    std::int32_t vy = 0;
};

class DesktopView {
public:
    void build(lv_obj_t* parent);
    void relayout();

    lv_obj_t* root() const { return root_; }
    lv_obj_t* launcher_btn() const { return launcher_btn_; }
    lv_obj_t* power_btn() const { return power_btn_; }

    core::StatusBarView& status_bar() { return status_bar_; }

private:
    static void on_root_event(lv_event_t* e);
    static void on_bg_timer(lv_timer_t* t);

    lv_obj_t* root_ = nullptr;
    core::StatusBarView status_bar_{};

    lv_obj_t* bg_ = nullptr;
    lv_timer_t* bg_timer_ = nullptr;
    std::array<DesktopParticle, SUNNY_UI_DESKTOP_BG_PARTICLES_COUNT> particles_{};
    bool particles_inited_ = false;
    lv_coord_t bounds_x0_ = 0;
    lv_coord_t bounds_y0_ = 0;
    lv_coord_t bounds_x1_ = 0;
    lv_coord_t bounds_y1_ = 0;
    lv_coord_t last_bg_w_ = 0;
    lv_coord_t last_bg_h_ = 0;

    lv_obj_t* content_host_ = nullptr;
    lv_obj_t* title_ = nullptr;

    lv_obj_t* launcher_btn_ = nullptr;
    lv_obj_t* launcher_icon_ = nullptr;
    lv_obj_t* launcher_label_ = nullptr;

    lv_obj_t* power_btn_ = nullptr;
    lv_obj_t* power_icon_ = nullptr;

    lv_obj_t* hint_root_ = nullptr;
    lv_obj_t* hint_label_ = nullptr;
};

} // namespace vsun::desktop
