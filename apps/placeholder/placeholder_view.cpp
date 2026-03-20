#include "apps/placeholder/placeholder_view.h"

#include "core/theme.h"
#include "sunny_resources.h"

namespace vsun::apps::placeholder {
namespace {

static constexpr lv_opa_t kIconOpacity = LV_OPA_80;

static lv_obj_t* create_chip(lv_obj_t* parent, const char* text)
{
    lv_obj_t* chip = lv_obj_create(parent);
    lv_obj_remove_style_all(chip);
    lv_obj_set_style_bg_opa(chip, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(chip, core::theme::divider(), 0);
    lv_obj_set_style_radius(chip, 8, 0);
    lv_obj_set_style_pad_left(chip, 8, 0);
    lv_obj_set_style_pad_right(chip, 8, 0);
    lv_obj_set_style_pad_top(chip, 4, 0);
    lv_obj_set_style_pad_bottom(chip, 4, 0);

    lv_obj_t* label = lv_label_create(chip);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, core::theme::secondary(), 0);
    lv_obj_center(label);

    return chip;
}

static void create_progress_bar(lv_obj_t* parent, int progress)
{
    lv_obj_t* progress_bg = lv_obj_create(parent);
    lv_obj_remove_style_all(progress_bg);
    lv_obj_set_size(progress_bg, 200, 4);
    lv_obj_set_style_bg_opa(progress_bg, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(progress_bg, core::theme::divider(), 0);
    lv_obj_set_style_radius(progress_bg, 4, 0);
    lv_obj_align(progress_bg, LV_ALIGN_BOTTOM_MID, 0, -16);

    lv_obj_t* progress_fg = lv_obj_create(progress_bg);
    lv_obj_remove_style_all(progress_fg);
    lv_obj_set_size(progress_fg, (200 * progress) / 100, 4);
    lv_obj_set_style_bg_opa(progress_fg, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(progress_fg, core::theme::primary(), 0);
    lv_obj_set_style_radius(progress_fg, 4, 0);
    lv_obj_align(progress_fg, LV_ALIGN_LEFT_MID, 0, 0);
}

} // namespace

void PlaceholderView::build(lv_obj_t* parent, const PlaceholderSpec& spec)
{
    root_ = lv_obj_create(parent);
    lv_obj_remove_style_all(root_);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(root_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(root_, 0, 0);
    lv_obj_clear_flag(root_, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* icon = lv_img_create(root_);
    lv_img_set_src(icon, spec.icon ? spec.icon : SUNNY_IMG(MYNAUI_CHECK_SOLID));
    lv_obj_set_style_img_recolor(icon, core::theme::primary(), 0);
    lv_obj_set_style_img_recolor_opa(icon, LV_OPA_COVER, 0);
    lv_obj_set_style_opa(icon, kIconOpacity, 0);
    lv_obj_align(icon, LV_ALIGN_CENTER, 0, -48);

    lv_obj_t* t = lv_label_create(root_);
    lv_label_set_text(t, spec.title ? spec.title : "APP");
    lv_obj_set_style_text_color(t, core::theme::primary(), 0);
    lv_obj_set_style_text_letter_space(t, 2, 0);
    lv_obj_align(t, LV_ALIGN_CENTER, 0, -18);

    lv_obj_t* sub = lv_label_create(root_);
    lv_label_set_text(sub, spec.subtitle ? spec.subtitle : "APPLICATION LOADED");
    lv_obj_set_style_text_color(sub, core::theme::hint(), 0);
    lv_obj_set_style_text_opa(sub, LV_OPA_90, 0);
    lv_obj_align(sub, LV_ALIGN_CENTER, 0, 2);

    lv_obj_t* detail = lv_label_create(root_);
    lv_label_set_text(detail, spec.detail ? spec.detail : "READY");
    lv_obj_set_style_text_color(detail, core::theme::secondary(), 0);
    lv_obj_set_style_text_opa(detail, LV_OPA_80, 0);
    lv_obj_align(detail, LV_ALIGN_CENTER, 0, 22);

    if(spec.chip_left || spec.chip_right) {
        lv_obj_t* row = lv_obj_create(root_);
        lv_obj_remove_style_all(row);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_column(row, 8, 0);
        lv_obj_set_size(row, lv_pct(100), LV_SIZE_CONTENT);
        lv_obj_align(row, LV_ALIGN_CENTER, 0, 50);

        if(spec.chip_left) create_chip(row, spec.chip_left);
        if(spec.chip_right) create_chip(row, spec.chip_right);
    }

    if(spec.progress >= 0) {
        int progress = spec.progress;
        if(progress > 100) progress = 100;
        create_progress_bar(root_, progress);
    }
}

} // namespace vsun::apps::placeholder
