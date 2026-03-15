#pragma once

#include "lvgl.h"

#include "desktop/desktop_model.h"
#include "desktop/desktop_view.h"

namespace vsun::desktop {

struct DesktopSliceCallbacks {
    void (*open_launcher)(void* user) = nullptr;
    void (*shutdown)(void* user) = nullptr;
    void* user = nullptr;
};

class DesktopSlice {
public:
    void build(lv_obj_t* parent);
    void set_callbacks(const DesktopSliceCallbacks& cb) { cb_ = cb; }

    lv_obj_t* root() const { return view_.root(); }

    void relayout();

private:
    static void on_root_size(lv_event_t* e);
    static void on_launcher_clicked(lv_event_t* e);
    static void on_power_clicked(lv_event_t* e);

    void do_open_launcher();
    void do_shutdown();

    DesktopModel model_{};
    DesktopView view_{};
    DesktopSliceCallbacks cb_{};
};

} // namespace vsun::desktop
