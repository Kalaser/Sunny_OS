#include "apps/placeholder/placeholder_view.h"

#include "core/theme.h"
#include "sunny_resources.h"

namespace vsun::apps::placeholder {

void PlaceholderView::build(lv_obj_t* parent, const char* title)
{
    root_ = lv_obj_create(parent);
    lv_obj_remove_style_all(root_);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(root_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(root_, 0, 0);
    lv_obj_clear_flag(root_, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* icon = lv_img_create(root_);
    lv_img_set_src(icon, SUNNY_IMG(MYNAUI_CHECK_SOLID));
    lv_obj_set_style_img_recolor(icon, core::theme::primary(), 0);
    lv_obj_set_style_img_recolor_opa(icon, LV_OPA_COVER, 0);
    lv_obj_set_style_opa(icon, LV_OPA_80, 0);
    lv_obj_align(icon, LV_ALIGN_CENTER, 0, -18);

    lv_obj_t* t = lv_label_create(root_);
    lv_label_set_text(t, title ? title : "APP");
    lv_obj_set_style_text_color(t, core::theme::primary(), 0);
    lv_obj_set_style_text_letter_space(t, 2, 0);
    lv_obj_align(t, LV_ALIGN_CENTER, 0, 8);

    lv_obj_t* sub = lv_label_create(root_);
    lv_label_set_text(sub, "APPLICATION LOADED");
    lv_obj_set_style_text_color(sub, core::theme::hint(), 0);
    lv_obj_align(sub, LV_ALIGN_CENTER, 0, 26);
}

} // namespace vsun::apps::placeholder
