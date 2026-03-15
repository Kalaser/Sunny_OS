#include "apps/maps/maps_view.h"

#include "core/theme.h"
#include "sunny_resources.h"

namespace vsun::apps::maps {

void MapsView::build(lv_obj_t* parent)
{
    root_ = lv_obj_create(parent);
    lv_obj_remove_style_all(root_);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(root_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(root_, 0, 0);
    lv_obj_clear_flag(root_, LV_OBJ_FLAG_SCROLLABLE);

    distance_ = lv_label_create(root_);
    lv_label_set_text(distance_, "250m");
    lv_obj_set_style_text_color(distance_, core::theme::primary(), 0);
    lv_obj_align(distance_, LV_ALIGN_CENTER, 0, -58);

    lv_obj_t* line1 = lv_label_create(root_);
    lv_label_set_text(line1, "Turn right onto");
    lv_obj_set_style_text_color(line1, core::theme::secondary(), 0);
    lv_obj_set_style_text_opa(line1, LV_OPA_90, 0);
    lv_obj_align(line1, LV_ALIGN_CENTER, 0, -26);

    lv_obj_t* line2 = lv_label_create(root_);
    lv_label_set_text(line2, "Main Street");
    lv_obj_set_style_text_color(line2, lv_color_white(), 0);
    lv_obj_align(line2, LV_ALIGN_CENTER, 0, -10);

    lv_obj_t* nav = lv_img_create(root_);
    lv_img_set_src(nav, SUNNY_IMG(MYNAUI_ARROW_RIGHT_SOLID));
    lv_obj_set_style_img_recolor(nav, core::theme::primary(), 0);
    lv_obj_set_style_img_recolor_opa(nav, LV_OPA_COVER, 0);
    lv_obj_align(nav, LV_ALIGN_CENTER, 0, 22);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, nav);
    lv_anim_set_exec_cb(&a, [](void* var, int32_t v) {
        lv_obj_align(static_cast<lv_obj_t*>(var), LV_ALIGN_CENTER, 0, v);
    });
    lv_anim_set_values(&a, 18, 28);
    lv_anim_set_time(&a, 900);
    lv_anim_set_playback_time(&a, 900);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);

    lv_obj_t* bar_bg = lv_obj_create(root_);
    lv_obj_remove_style_all(bar_bg);
    lv_obj_set_size(bar_bg, 216, 4);
    lv_obj_set_style_bg_opa(bar_bg, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(bar_bg, core::theme::divider(), 0);
    lv_obj_set_style_radius(bar_bg, 4, 0);
    lv_obj_align(bar_bg, LV_ALIGN_BOTTOM_MID, 0, -14);

    lv_obj_t* bar = lv_obj_create(bar_bg);
    lv_obj_remove_style_all(bar);
    lv_obj_set_size(bar, (int)(216 * 0.6f), 4);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(bar, core::theme::primary(), 0);
    lv_obj_set_style_radius(bar, 4, 0);
    lv_obj_align(bar, LV_ALIGN_LEFT_MID, 0, 0);
}

void MapsView::set_distance(int meters)
{
    if(!distance_) return;
    char buf[16]{};
    lv_snprintf(buf, sizeof(buf), "%dm", meters);
    lv_label_set_text(distance_, buf);
}

} // namespace vsun::apps::maps
