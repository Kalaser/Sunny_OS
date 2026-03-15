#pragma once

#include <cstddef>

#include "lvgl.h"

#include "core/app_entry.h"
#include "core/status_bar_view.h"
#include "launcher/launcher_model.h"
#include "launcher/launcher_view.h"

namespace vsun::launcher {

struct LauncherSliceCallbacks {
    void (*open_app_by_id)(const char* app_id, void* user) = nullptr;
    void* user = nullptr;

    // Optional: swipe down in launcher to return to Desktop.
    void (*go_desktop)(void* user) = nullptr;
};

class LauncherSlice {
public:
    void build(lv_obj_t* parent, const core::AppEntry* apps, size_t app_count);
    void set_callbacks(const LauncherSliceCallbacks& cb);

    lv_obj_t* root() const { return root_; }
    int selected_index() const { return model_.selected_index(); }

    void relayout(bool animate);

private:
    static void on_select(int index, void* user);
    static void on_open(int index, void* user);
    static void on_move_left(void* user);
    static void on_move_right(void* user);
    static void on_back_clicked(lv_event_t* e);

    void do_select(int index);
    void do_open(int index);
    void do_move_left();
    void do_move_right();
    void do_go_desktop();

    static void on_root_size(lv_event_t* e);
    static void on_root_gesture(lv_event_t* e);

    lv_obj_t* root_ = nullptr;
    core::StatusBarView status_bar_{};

    lv_obj_t* carousel_host_ = nullptr;
    LauncherModel model_{};
    LauncherView view_{};

    lv_obj_t* hint_root_ = nullptr;
    lv_obj_t* hint_label_ = nullptr;
    lv_obj_t* back_btn_ = nullptr;

    const core::AppEntry* apps_ = nullptr;
    size_t app_count_ = 0;
    LauncherSliceCallbacks cb_{};
};

} // namespace vsun::launcher
