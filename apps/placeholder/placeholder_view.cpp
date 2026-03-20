#include "apps/placeholder/placeholder_view.h"

#include "core/theme.h"
#include "sunny_resources.h"

namespace vsun::apps::placeholder {
namespace {

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

static void create_items_panel(lv_obj_t* parent, const PlaceholderSpec& spec, lv_coord_t w, lv_coord_t h)
{
    if(!spec.items || spec.item_count == 0) return;

    lv_coord_t panel_w = (lv_coord_t)(w * 78 / 100);
    if(panel_w > 320) panel_w = 320;
    if(panel_w < 220) panel_w = 220;

    lv_coord_t row_h = (lv_coord_t)(h / 7);
    if(row_h < 28) row_h = 28;
    if(row_h > 42) row_h = 42;

    lv_coord_t panel_h = (lv_coord_t)(spec.item_count * row_h + (spec.item_count - 1) * 8 + 16);

    lv_obj_t* panel = lv_obj_create(parent);
    lv_obj_remove_style_all(panel);
    lv_obj_set_size(panel, panel_w, panel_h);
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    lv_obj_set_style_border_color(panel, core::theme::divider(), 0);
    lv_obj_set_style_border_opa(panel, LV_OPA_70, 0);
    lv_obj_set_style_radius(panel, 14, 0);
    lv_obj_set_style_pad_all(panel, 8, 0);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(panel, 8, 0);
    lv_obj_align(panel, LV_ALIGN_CENTER, 0, 16);

    for(std::uint8_t i = 0; i < spec.item_count; ++i) {
        const PlaceholderItem& item = spec.items[i];
        lv_obj_t* row = lv_obj_create(panel);
        lv_obj_remove_style_all(row);
        lv_obj_set_size(row, lv_pct(100), row_h);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 1, 0);
        lv_obj_set_style_border_color(row, core::theme::divider(), 0);
        lv_obj_set_style_border_opa(row, LV_OPA_80, 0);
        lv_obj_set_style_radius(row, 10, 0);
        lv_obj_set_style_pad_left(row, 10, 0);
        lv_obj_set_style_pad_right(row, 10, 0);
        lv_obj_set_style_pad_top(row, 0, 0);
        lv_obj_set_style_pad_bottom(row, 0, 0);

        if(item.icon) {
            lv_obj_t* icon = lv_img_create(row);
            lv_img_set_src(icon, item.icon);
            lv_obj_set_style_img_recolor(icon, core::theme::primary(), 0);
            lv_obj_set_style_img_recolor_opa(icon, LV_OPA_COVER, 0);
            lv_obj_align(icon, LV_ALIGN_LEFT_MID, 0, 0);
        }

        lv_obj_t* key = lv_label_create(row);
        lv_label_set_text(key, item.key ? item.key : "-");
        lv_obj_set_style_text_color(key, core::theme::secondary(), 0);
        lv_obj_align(key, LV_ALIGN_LEFT_MID, item.icon ? 26 : 0, 0);

        lv_obj_t* value = lv_label_create(row);
        lv_label_set_text(value, item.value ? item.value : "-");
        lv_obj_set_style_text_color(value, core::theme::primary(), 0);
        lv_obj_align(value, LV_ALIGN_RIGHT_MID, 0, 0);
    }
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

    const lv_coord_t w = lv_obj_get_content_width(root_);
    const lv_coord_t h = lv_obj_get_content_height(root_);

    lv_obj_t* icon = lv_img_create(root_);
    lv_img_set_src(icon, spec.icon ? spec.icon : SUNNY_IMG(MYNAUI_CHECK_SOLID));
    lv_obj_set_style_img_recolor(icon, core::theme::primary(), 0);
    lv_obj_set_style_img_recolor_opa(icon, LV_OPA_COVER, 0);
    lv_obj_set_style_opa(icon, LV_OPA_85, 0);
    lv_obj_align(icon, LV_ALIGN_CENTER, 0, -h / 3);

    lv_obj_t* t = lv_label_create(root_);
    lv_label_set_text(t, spec.title ? spec.title : "APP");
    lv_obj_set_style_text_color(t, core::theme::primary(), 0);
    lv_obj_set_style_text_letter_space(t, 2, 0);
    lv_obj_align(t, LV_ALIGN_CENTER, 0, -h / 4);

    lv_obj_t* sub = lv_label_create(root_);
    lv_label_set_text(sub, spec.subtitle ? spec.subtitle : "APPLICATION LOADED");
    lv_obj_set_style_text_color(sub, core::theme::hint(), 0);
    lv_obj_set_style_text_opa(sub, LV_OPA_90, 0);
    lv_obj_align(sub, LV_ALIGN_CENTER, 0, -h / 5 + 26);

    lv_obj_t* detail = lv_label_create(root_);
    lv_label_set_text(detail, spec.detail ? spec.detail : "READY");
    lv_obj_set_style_text_color(detail, core::theme::secondary(), 0);
    lv_obj_set_style_text_opa(detail, LV_OPA_80, 0);
    lv_obj_align(detail, LV_ALIGN_CENTER, 0, -h / 5 + 46);

    create_items_panel(root_, spec, w, h);

    if(spec.chip_left || spec.chip_right) {
        lv_obj_t* row = lv_obj_create(root_);
        lv_obj_remove_style_all(row);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_column(row, 8, 0);
        lv_obj_set_size(row, lv_pct(100), LV_SIZE_CONTENT);
        lv_obj_align(row, LV_ALIGN_BOTTOM_MID, 0, (spec.progress >= 0) ? -34 : -16);

        if(spec.chip_left) create_chip(row, spec.chip_left);
        if(spec.chip_right) create_chip(row, spec.chip_right);
    }

    if(spec.progress >= 0) {
        int progress = spec.progress;
        if(progress > 100) progress = 100;

        lv_coord_t bar_w = (lv_coord_t)(w * 55 / 100);
        if(bar_w > 240) bar_w = 240;
        if(bar_w < 140) bar_w = 140;

        lv_obj_t* bar_bg = lv_obj_create(root_);
        lv_obj_remove_style_all(bar_bg);
        lv_obj_set_size(bar_bg, bar_w, 4);
        lv_obj_set_style_bg_opa(bar_bg, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(bar_bg, core::theme::divider(), 0);
        lv_obj_set_style_radius(bar_bg, 4, 0);
        lv_obj_align(bar_bg, LV_ALIGN_BOTTOM_MID, 0, -16);

        lv_obj_t* bar = lv_obj_create(bar_bg);
        lv_obj_remove_style_all(bar);
        lv_obj_set_size(bar, (bar_w * progress) / 100, 4);
        lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(bar, core::theme::primary(), 0);
        lv_obj_set_style_radius(bar, 4, 0);
        lv_obj_align(bar, LV_ALIGN_LEFT_MID, 0, 0);
    }
}

} // namespace vsun::apps::placeholder
