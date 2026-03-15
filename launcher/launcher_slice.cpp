#include "launcher/launcher_slice.h"

#include "core/theme.h"
#include "config/ui_config.h"
#include "sunny_resources.h"

namespace vsun::launcher {
namespace {

static const char* kHintText = "Tap to Open";

} // namespace

void LauncherSlice::set_callbacks(const LauncherSliceCallbacks& cb)
{
    cb_ = cb;

    // build() can run before callbacks are wired, so update optional UI visibility here too.
    if(back_btn_) {
        if(cb_.go_desktop) lv_obj_clear_flag(back_btn_, LV_OBJ_FLAG_HIDDEN);
        else lv_obj_add_flag(back_btn_, LV_OBJ_FLAG_HIDDEN);
    }
}

void LauncherSlice::on_select(int index, void* user) { static_cast<LauncherSlice*>(user)->do_select(index); }
void LauncherSlice::on_open(int index, void* user) { static_cast<LauncherSlice*>(user)->do_open(index); }
void LauncherSlice::on_move_left(void* user) { static_cast<LauncherSlice*>(user)->do_move_left(); }
void LauncherSlice::on_move_right(void* user) { static_cast<LauncherSlice*>(user)->do_move_right(); }

void LauncherSlice::on_back_clicked(lv_event_t* e)
{
    auto* self = static_cast<LauncherSlice*>(lv_event_get_user_data(e));
    if(!self) return;
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) self->do_go_desktop();
}

void LauncherSlice::on_root_size(lv_event_t* e)
{
    auto* self = static_cast<LauncherSlice*>(lv_event_get_user_data(e));
    if(!self) return;
    self->relayout(false);
}

void LauncherSlice::on_root_gesture(lv_event_t* e)
{
    auto* self = static_cast<LauncherSlice*>(lv_event_get_user_data(e));
    if(!self) return;

    lv_indev_t* indev = lv_indev_get_act();
    if(!indev) return;

    if(lv_indev_get_gesture_dir(indev) == LV_DIR_BOTTOM) self->do_go_desktop();
}
void LauncherSlice::build(lv_obj_t* parent, const core::AppEntry* apps, size_t app_count)
{
    apps_ = apps;
    app_count_ = app_count;

    root_ = lv_obj_create(parent);
    lv_obj_remove_style_all(root_);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(root_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(root_, 0, 0);
    lv_obj_clear_flag(root_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(root_, on_root_size, LV_EVENT_SIZE_CHANGED, this);
    lv_obj_add_event_cb(root_, on_root_gesture, LV_EVENT_GESTURE, this);

    status_bar_.build(root_);

    carousel_host_ = lv_obj_create(root_);
    lv_obj_remove_style_all(carousel_host_);
    lv_obj_set_style_bg_opa(carousel_host_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(carousel_host_, 0, 0);
    lv_obj_clear_flag(carousel_host_, LV_OBJ_FLAG_SCROLLABLE);

    view_.build(carousel_host_, apps_, app_count_);
    LauncherViewCallbacks vcb{};
    vcb.on_select = &LauncherSlice::on_select;
    vcb.on_open = &LauncherSlice::on_open;
    vcb.on_move_left = &LauncherSlice::on_move_left;
    vcb.on_move_right = &LauncherSlice::on_move_right;
    vcb.user = this;
    view_.set_callbacks(vcb);

    // Bottom divider
    lv_obj_t* divider = lv_obj_create(root_);
    lv_obj_remove_style_all(divider);
    lv_obj_set_size(divider, lv_pct(100), 1);
    lv_obj_set_style_bg_opa(divider, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(divider, core::theme::divider(), 0);
    lv_obj_align(divider, LV_ALIGN_BOTTOM_MID, 0, -SUNNY_UI_HINT_BAR_H);

    hint_root_ = lv_obj_create(root_);
    lv_obj_remove_style_all(hint_root_);
    lv_obj_set_size(hint_root_, lv_pct(100), SUNNY_UI_HINT_BAR_H);
    lv_obj_set_style_bg_opa(hint_root_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(hint_root_, 0, 0);
    lv_obj_align(hint_root_, LV_ALIGN_BOTTOM_MID, 0, 0);

    back_btn_ = lv_btn_create(hint_root_);
    lv_obj_set_size(back_btn_, SUNNY_UI_LAUNCHER_BACK_BTN_W, SUNNY_UI_LAUNCHER_BACK_BTN_H);
    lv_obj_align(back_btn_, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_bg_opa(back_btn_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(back_btn_, 1, 0);
    lv_obj_set_style_border_color(back_btn_, core::theme::divider(), 0);
    lv_obj_set_style_border_opa(back_btn_, LV_OPA_60, 0);
    lv_obj_set_style_radius(back_btn_, 11, 0);
    lv_obj_add_event_cb(back_btn_, on_back_clicked, LV_EVENT_CLICKED, this);
    if(!cb_.go_desktop) lv_obj_add_flag(back_btn_, LV_OBJ_FLAG_HIDDEN);

    lv_obj_set_flex_flow(back_btn_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(back_btn_, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_left(back_btn_, 8, 0);
    lv_obj_set_style_pad_right(back_btn_, 8, 0);
    lv_obj_set_style_pad_top(back_btn_, 0, 0);
    lv_obj_set_style_pad_bottom(back_btn_, 0, 0);
    lv_obj_set_style_pad_column(back_btn_, 6, 0);

    lv_obj_t* back_icon = lv_img_create(back_btn_);
    lv_img_set_src(back_icon, SUNNY_IMG(MYNAUI16_ARROW_LEFT_SOLID));
    lv_obj_set_style_img_recolor(back_icon, core::theme::hint(), 0);
    lv_obj_set_style_img_recolor_opa(back_icon, LV_OPA_COVER, 0);

    lv_obj_t* back_txt = lv_label_create(back_btn_);
    lv_label_set_text(back_txt, "DESKTOP");
    lv_obj_set_style_text_color(back_txt, core::theme::hint(), 0);

    hint_label_ = lv_label_create(hint_root_);
    lv_label_set_text(hint_label_, kHintText);
    lv_obj_set_style_text_color(hint_label_, core::theme::hint(), 0);
    lv_obj_set_style_text_letter_space(hint_label_, 2, 0);
    lv_obj_center(hint_label_);

    // hint breathing
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, hint_label_);
    lv_anim_set_exec_cb(&a, [](void* var, int32_t v) {
        lv_obj_set_style_opa(static_cast<lv_obj_t*>(var), (lv_opa_t)v, 0);
    });
    lv_anim_set_values(&a, 80, 200);
    lv_anim_set_time(&a, 1200);
    lv_anim_set_playback_time(&a, 1200);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);

    relayout(false);
}

void LauncherSlice::relayout(bool animate)
{
    if(!root_) return;

    const lv_coord_t w = lv_obj_get_width(root_);
    const lv_coord_t h = lv_obj_get_height(root_);

    const lv_coord_t top = 29;    // status+divider
    const lv_coord_t bottom = 28; // hint area
    const lv_coord_t ch = (h > (top + bottom)) ? (h - top - bottom) : 0;

    if(carousel_host_) {
        lv_obj_set_pos(carousel_host_, 0, top);
        lv_obj_set_size(carousel_host_, w, ch);
    }

    // Ensure the view has a concrete size before it reads width/height for layout.
    // Percent sizing + hidden/show transitions can leave the cached size at 0 here.
    if(view_.root()) {
        lv_obj_set_pos(view_.root(), 0, 0);
        lv_obj_set_size(view_.root(), w, ch);
        lv_obj_update_layout(view_.root());
    }

    view_.relayout(model_.selected_index(), apps_, app_count_, animate);
}

void LauncherSlice::do_select(int index)
{
    if(index < 0) index = 0;
    if(index >= (int)app_count_) index = (int)app_count_ - 1;

    if(index == model_.selected_index()) return;

    model_.set_selected_index(index);
    relayout(true);
}

void LauncherSlice::do_open(int index)
{
    if(index != model_.selected_index()) return;
    if(index < 0 || (size_t)index >= app_count_) return;

    if(cb_.open_app_by_id) cb_.open_app_by_id(apps_[index].id, cb_.user);
}

void LauncherSlice::do_move_left()
{
    const int before = model_.selected_index();
    model_.move_left();
    if(model_.selected_index() != before) relayout(true);
}

void LauncherSlice::do_move_right()
{
    const int before = model_.selected_index();
    model_.move_right((int)app_count_ - 1);
    if(model_.selected_index() != before) relayout(true);
}

void LauncherSlice::do_go_desktop()
{
    if(cb_.go_desktop) cb_.go_desktop(cb_.user);
}

} // namespace vsun::launcher
