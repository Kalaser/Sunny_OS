#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "lvgl.h"

#include "core/app_entry.h"

namespace vsun::launcher {

struct LauncherViewCallbacks {
    void (*on_select)(int index, void* user) = nullptr;
    void (*on_open)(int index, void* user) = nullptr;
    void (*on_move_left)(void* user) = nullptr;
    void (*on_move_right)(void* user) = nullptr;
    void* user = nullptr;
};

class LauncherView {
public:
    void build(lv_obj_t* parent, const core::AppEntry* apps, size_t app_count);
    void set_callbacks(const LauncherViewCallbacks& cb) { cb_ = cb; }

    void relayout(int selected_index, const core::AppEntry* apps, size_t app_count, bool animate);
    lv_obj_t* root() const { return root_; }

private:
    struct ItemUi {
        lv_obj_t* item = nullptr;
        lv_obj_t* circle = nullptr;
        lv_obj_t* icon = nullptr;
        lv_obj_t* label = nullptr;
        lv_obj_t* ring = nullptr;
    };

    static void on_item_clicked(lv_event_t* e);
    static void on_gesture(lv_event_t* e);

    void handle_item_clicked(int idx);
    void handle_gesture(lv_dir_t dir);

    static void anim_set_x(void* var, int32_t v);
    static void anim_set_y(void* var, int32_t v);
    static void anim_set_opa(void* var, int32_t v);
    static void anim_set_scale(void* var, int32_t v);

    void animate_obj_xy(lv_obj_t* obj, lv_coord_t x, lv_coord_t y);
    void animate_obj_scale(lv_obj_t* obj, int32_t scale_256);
    void animate_obj_opa(lv_obj_t* obj, lv_opa_t opa);

    lv_obj_t* root_ = nullptr;
    const core::AppEntry* apps_ = nullptr;
    size_t app_count_ = 0;
    LauncherViewCallbacks cb_{};
    std::vector<ItemUi> items_;
    int selected_index_ = 1;
};

} // namespace vsun::launcher

