#include "launcher/launcher_view.h"

#include <cmath>

#include "core/theme.h"
#include "sunny_resources.h"
#include "runtime/vsun_layout.h"
#include "config/ui_config.h"

namespace vsun::launcher {
namespace {

static constexpr int kBaseIconPx = SUNNY_UI_LAUNCHER_ICON_PX;
static constexpr int kBaseItemHeight = SUNNY_UI_LAUNCHER_ITEM_H;

static void start_ring_spin(lv_obj_t* ring)
{
    if(!ring) return;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ring);
    lv_anim_set_exec_cb(&a, [](void* var, int32_t v) {
        // LVGL v8 uses "transform_angle" in 0.1 degree units.
        lv_obj_set_style_transform_angle(static_cast<lv_obj_t*>(var), (lv_coord_t)v, 0);
    });
    lv_anim_set_values(&a, 0, 3600);
    lv_anim_set_time(&a, 8000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);
}

} // namespace

void LauncherView::anim_set_x(void* var, int32_t v) { lv_obj_set_x(static_cast<lv_obj_t*>(var), (lv_coord_t)v); }
void LauncherView::anim_set_y(void* var, int32_t v) { lv_obj_set_y(static_cast<lv_obj_t*>(var), (lv_coord_t)v); }
void LauncherView::anim_set_opa(void* var, int32_t v) { lv_obj_set_style_opa(static_cast<lv_obj_t*>(var), (lv_opa_t)v, 0); }
void LauncherView::anim_set_scale(void* var, int32_t v)
{
    lv_obj_t* obj = static_cast<lv_obj_t*>(var);
    // LVGL v8 provides uniform zoom (256 == 1.0x).
    lv_obj_set_style_transform_zoom(obj, (lv_coord_t)v, 0);
}

void LauncherView::animate_obj_xy(lv_obj_t* obj, lv_coord_t x, lv_coord_t y)
{
    if(!obj) return;

    lv_anim_del(obj, anim_set_x);
    lv_anim_del(obj, anim_set_y);

    lv_anim_t ax;
    lv_anim_init(&ax);
    lv_anim_set_var(&ax, obj);
    lv_anim_set_exec_cb(&ax, anim_set_x);
    lv_anim_set_values(&ax, lv_obj_get_x(obj), x);
    lv_anim_set_time(&ax, 220);
    lv_anim_set_path_cb(&ax, lv_anim_path_ease_out);
    lv_anim_start(&ax);

    lv_anim_t ay;
    lv_anim_init(&ay);
    lv_anim_set_var(&ay, obj);
    lv_anim_set_exec_cb(&ay, anim_set_y);
    lv_anim_set_values(&ay, lv_obj_get_y(obj), y);
    lv_anim_set_time(&ay, 220);
    lv_anim_set_path_cb(&ay, lv_anim_path_ease_out);
    lv_anim_start(&ay);
}

void LauncherView::animate_obj_scale(lv_obj_t* obj, int32_t scale_256)
{
    if(!obj) return;

    lv_anim_del(obj, anim_set_scale);

    const int32_t cur = (int32_t)lv_obj_get_style_transform_zoom(obj, 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, anim_set_scale);
    lv_anim_set_values(&a, cur, scale_256);
    lv_anim_set_time(&a, 220);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

void LauncherView::animate_obj_opa(lv_obj_t* obj, lv_opa_t opa)
{
    if(!obj) return;

    lv_anim_del(obj, anim_set_opa);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, anim_set_opa);
    lv_anim_set_values(&a, (int32_t)lv_obj_get_style_opa(obj, 0), opa);
    lv_anim_set_time(&a, 180);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

void LauncherView::on_item_clicked(lv_event_t* e)
{
    auto* self = static_cast<LauncherView*>(lv_event_get_user_data(e));
    if(!self) return;

    lv_obj_t* target = (lv_obj_t*)lv_event_get_current_target(e);
    const int idx = (int)(intptr_t)lv_obj_get_user_data(target);
    self->handle_item_clicked(idx);
}

void LauncherView::on_gesture(lv_event_t* e)
{
    auto* self = static_cast<LauncherView*>(lv_event_get_user_data(e));
    if(!self) return;

    lv_indev_t* indev = lv_indev_get_act();
    if(!indev) return;

    self->handle_gesture(lv_indev_get_gesture_dir(indev));
}

void LauncherView::handle_item_clicked(int idx)
{
    if(idx < 0 || (size_t)idx >= app_count_) return;

    if(idx == selected_index_) {
        if(cb_.on_open) cb_.on_open(idx, cb_.user);
        return;
    }

    if(cb_.on_select) cb_.on_select(idx, cb_.user);
}

void LauncherView::handle_gesture(lv_dir_t dir)
{
    if(dir == LV_DIR_LEFT) {
        if(cb_.on_move_right) cb_.on_move_right(cb_.user);
    } else if(dir == LV_DIR_RIGHT) {
        if(cb_.on_move_left) cb_.on_move_left(cb_.user);
    }
}

void LauncherView::build(lv_obj_t* parent, const core::AppEntry* apps, size_t app_count)
{
    apps_ = apps;
    app_count_ = app_count;

    root_ = lv_obj_create(parent);
    lv_obj_remove_style_all(root_);
    lv_obj_set_style_bg_opa(root_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(root_, 0, 0);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_align(root_, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_flag(root_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);

    lv_obj_add_event_cb(root_, on_gesture, LV_EVENT_GESTURE, this);

    items_.clear();
    items_.reserve(app_count);

    for(size_t i = 0; i < app_count; i++) {
        ItemUi ui;

        ui.item = lv_obj_create(root_);
        lv_obj_remove_style_all(ui.item);
        lv_obj_set_size(ui.item, kBaseIconPx, kBaseItemHeight);
        lv_obj_set_style_bg_opa(ui.item, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(ui.item, 0, 0);
        lv_obj_add_flag(ui.item, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
        lv_obj_add_flag(ui.item, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(ui.item, LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_add_flag(ui.item, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_set_user_data(ui.item, (void*)(intptr_t)i);
        lv_obj_add_event_cb(ui.item, on_item_clicked, LV_EVENT_CLICKED, this);
        lv_obj_add_event_cb(ui.item, on_gesture, LV_EVENT_GESTURE, this);

        // Pivot for smooth scaling.
        lv_obj_set_style_transform_pivot_x(ui.item, kBaseIconPx / 2, 0);
        lv_obj_set_style_transform_pivot_y(ui.item, kBaseIconPx / 2, 0);
        anim_set_scale(ui.item, LV_IMG_ZOOM_NONE);

        ui.circle = lv_obj_create(ui.item);
        lv_obj_remove_style_all(ui.circle);
        lv_obj_set_size(ui.circle, kBaseIconPx, kBaseIconPx);
        lv_obj_set_style_radius(ui.circle, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_opa(ui.circle, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(ui.circle, 2, 0);
        lv_obj_set_style_border_color(ui.circle, core::theme::primary(), 0);
        lv_obj_set_style_border_opa(ui.circle, LV_OPA_60, 0);
        lv_obj_clear_flag(ui.circle, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(ui.circle, LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_add_flag(ui.circle, LV_OBJ_FLAG_EVENT_BUBBLE);

        ui.icon = lv_img_create(ui.circle);
        lv_img_set_src(ui.icon, apps[i].icon_img ? apps[i].icon_img : SUNNY_IMG(MYNAUI_GRID_SOLID));
        lv_img_set_zoom(ui.icon, 384); // 24px -> 36px (1.5x)
        lv_obj_set_style_img_recolor(ui.icon, core::theme::secondary(), 0);
        lv_obj_set_style_img_recolor_opa(ui.icon, LV_OPA_COVER, 0);
        lv_obj_center(ui.icon);
        lv_obj_clear_flag(ui.icon, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(ui.icon, LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_add_flag(ui.icon, LV_OBJ_FLAG_EVENT_BUBBLE);

        ui.label = lv_label_create(ui.item);
        lv_label_set_text(ui.label, apps[i].label ? apps[i].label : "");
        lv_obj_set_style_text_color(ui.label, core::theme::primary(), 0);
        lv_obj_set_style_text_letter_space(ui.label, 2, 0);
        lv_obj_align(ui.label, LV_ALIGN_TOP_MID, 0, kBaseIconPx + 8);
        lv_obj_add_flag(ui.label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui.label, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(ui.label, LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_add_flag(ui.label, LV_OBJ_FLAG_EVENT_BUBBLE);

        ui.ring = lv_obj_create(ui.item);
        lv_obj_remove_style_all(ui.ring);
        lv_obj_set_size(ui.ring, kBaseIconPx + 12, kBaseIconPx + 12);
        lv_obj_set_style_radius(ui.ring, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_opa(ui.ring, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(ui.ring, 1, 0);
        lv_obj_set_style_border_color(ui.ring, core::theme::primary(), 0);
        lv_obj_set_style_border_opa(ui.ring, LV_OPA_40, 0);
        lv_obj_align(ui.ring, LV_ALIGN_TOP_MID, 0, -6);
        lv_obj_clear_flag(ui.ring, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(ui.ring, LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_add_flag(ui.ring, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_add_flag(ui.ring, LV_OBJ_FLAG_HIDDEN);
        start_ring_spin(ui.ring);

        items_.push_back(ui);
    }
}

void LauncherView::relayout(int selected_index, const core::AppEntry* apps, size_t app_count, bool animate)
{
    apps_ = apps;
    app_count_ = app_count;
    selected_index_ = selected_index;

    if(!root_) return;

    const lv_coord_t w = lv_obj_get_width(root_);
    const lv_coord_t h = lv_obj_get_height(root_);

    const int radius = (int)std::lround((double)w * ((double)SUNNY_UI_LAUNCHER_RADIUS_FACTOR_X100 / 100.0));
    const int angle_step = SUNNY_UI_LAUNCHER_ANGLE_STEP_DEG;

    const int center_x = (int)std::lround((double)w * ((double)SUNNY_UI_LAUNCHER_CENTER_X_FACTOR_X100 / 100.0));
    const int center_y = (int)std::lround((double)h * ((double)SUNNY_UI_LAUNCHER_CENTER_Y_FACTOR_X100 / 100.0));

    for(size_t i = 0; i < items_.size(); i++) {
        ItemUi& ui = items_[i];
        if(!ui.item) continue;

        const auto l = vsun::compute_carousel_item_layout((int)i, selected_index, radius, angle_step);
        const bool selected = ((int)i == selected_index);
        const bool was_hidden = lv_obj_has_flag(ui.item, LV_OBJ_FLAG_HIDDEN);

        if(!l.visible) {
            lv_obj_add_flag(ui.item, LV_OBJ_FLAG_HIDDEN);
            continue;
        }

        const int base_half = kBaseIconPx / 2;
        const lv_coord_t pos_x = (lv_coord_t)(center_x + l.x - base_half);
        const lv_coord_t pos_y = (lv_coord_t)(center_y + l.y - base_half);

        const int32_t scale = (int32_t)std::lround((double)l.size * (double)LV_IMG_ZOOM_NONE / (double)kBaseIconPx);

        // Important: if an item was previously hidden, its last (x,y) may be (0,0).
        // If we clear HIDDEN before setting a sane position, it can flash a "ghost" at the top-left.
        // So we pre-position/pre-style hidden->visible items.
        if(was_hidden) {
            lv_obj_set_pos(ui.item, pos_x, pos_y);
            anim_set_scale(ui.item, scale);
            lv_obj_set_style_opa(ui.item, 0, 0);
            lv_obj_clear_flag(ui.item, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_clear_flag(ui.item, LV_OBJ_FLAG_HIDDEN);
        }

        // Visual state
        lv_obj_set_style_border_opa(ui.circle, selected ? LV_OPA_COVER : LV_OPA_60, 0);
        lv_obj_set_style_bg_opa(ui.circle, selected ? LV_OPA_20 : LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_color(ui.circle, core::theme::primary(), 0);
        lv_obj_set_style_img_recolor(ui.icon, selected ? core::theme::primary() : core::theme::secondary(), 0);

        if(selected) {
            lv_obj_clear_flag(ui.label, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui.ring, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(ui.label, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui.ring, LV_OBJ_FLAG_HIDDEN);
        }

        if(animate) {
            if(!was_hidden) {
                animate_obj_xy(ui.item, pos_x, pos_y);
                animate_obj_scale(ui.item, scale);
            }
            animate_obj_opa(ui.item, (lv_opa_t)l.opa);
        } else {
            lv_obj_set_pos(ui.item, pos_x, pos_y);
            anim_set_scale(ui.item, scale);
            lv_obj_set_style_opa(ui.item, (lv_opa_t)l.opa, 0);
        }
    }

    // Fix z-order: farther items first, selected last (on top).
    // Without this, items can overlap in an unintuitive order depending on creation index.
    for(int d = 3; d >= 0; --d) {
        for(size_t i = 0; i < items_.size(); i++) {
            const int abs_delta = (int)std::lround(std::fabs((double)((int)i - selected_index)));
            if(abs_delta != d) continue;
            ItemUi& ui = items_[i];
            if(!ui.item) continue;
            if(lv_obj_has_flag(ui.item, LV_OBJ_FLAG_HIDDEN)) continue;
            lv_obj_move_foreground(ui.item);
        }
    }
}

} // namespace vsun::launcher



