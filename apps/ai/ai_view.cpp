#include "apps/ai/ai_view.h"

#include "core/theme.h"

namespace vsun::apps::ai {

void AiView::build(lv_obj_t* parent)
{
    root_ = lv_obj_create(parent);
    lv_obj_remove_style_all(root_);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(root_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(root_, 0, 0);
    lv_obj_clear_flag(root_, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* frame = lv_obj_create(root_);
    lv_obj_remove_style_all(frame);
    lv_obj_set_size(frame, 128, 80);
    lv_obj_set_style_bg_opa(frame, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(frame, 1, 0);
    lv_obj_set_style_border_color(frame, core::theme::primary(), 0);
    lv_obj_set_style_border_opa(frame, LV_OPA_60, 0);
    lv_obj_set_style_radius(frame, 8, 0);
    lv_obj_align(frame, LV_ALIGN_CENTER, 0, -34);

    lv_obj_t* scan_tag = lv_label_create(root_);
    lv_label_set_text(scan_tag, "SCANNING...");
    lv_obj_set_style_bg_opa(scan_tag, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(scan_tag, core::theme::primary(), 0);
    lv_obj_set_style_text_color(scan_tag, lv_color_black(), 0);
    lv_obj_set_style_pad_left(scan_tag, 4, 0);
    lv_obj_set_style_pad_right(scan_tag, 4, 0);
    lv_obj_set_style_pad_top(scan_tag, 1, 0);
    lv_obj_set_style_pad_bottom(scan_tag, 1, 0);
    lv_obj_align_to(scan_tag, frame, LV_ALIGN_OUT_TOP_LEFT, -2, -2);

    // bars
    const int bars = 8;
    const int bar_w = 6;
    const int gap = 4;
    const int base_h = 10;
    const int max_h = 32;
    const int total_w = bars * bar_w + (bars - 1) * gap;

    lv_obj_t* bar_row = lv_obj_create(root_);
    lv_obj_remove_style_all(bar_row);
    lv_obj_set_size(bar_row, total_w, max_h);
    lv_obj_set_style_bg_opa(bar_row, LV_OPA_TRANSP, 0);
    lv_obj_align(bar_row, LV_ALIGN_CENTER, 0, 30);

    for(int i = 0; i < bars; i++) {
        lv_obj_t* b = lv_obj_create(bar_row);
        lv_obj_remove_style_all(b);
        lv_obj_set_size(b, bar_w, base_h);
        lv_obj_set_style_bg_opa(b, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(b, core::theme::primary(), 0);
        lv_obj_set_style_radius(b, 3, 0);
        lv_obj_set_pos(b, i * (bar_w + gap), max_h - base_h);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, b);
        lv_anim_set_exec_cb(&a, [](void* var, int32_t v) {
            lv_obj_t* obj = static_cast<lv_obj_t*>(var);
            const int h = (int)v;
            lv_obj_set_height(obj, h);
            lv_obj_set_y(obj, max_h - h);
        });
        lv_anim_set_values(&a, base_h, max_h);
        lv_anim_set_time(&a, 700);
        lv_anim_set_playback_time(&a, 700);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_delay(&a, (uint32_t)(i * 90));
        lv_anim_start(&a);
    }

    lv_obj_t* quote = lv_label_create(root_);
    lv_label_set_text(quote, "\"Object identified: Coffee Cup\"");
    lv_obj_set_style_text_color(quote, core::theme::secondary(), 0);
    lv_obj_set_style_text_opa(quote, LV_OPA_80, 0);
    lv_obj_align(quote, LV_ALIGN_BOTTOM_MID, 0, -18);
}

} // namespace vsun::apps::ai
