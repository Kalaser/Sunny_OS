#include "pages/shutdown_page.h"

#include <cstddef>

#include "config/ui_config.h"
#include "core/theme.h"
#include "port/system.h"

namespace vsun::pages {

void ShutdownPage::anim_set_arc_value(void* var, int32_t v)
{
    lv_arc_set_value(static_cast<lv_obj_t*>(var), (int16_t)v);
}

void ShutdownPage::anim_set_root_opa(void* var, int32_t v)
{
    lv_obj_set_style_opa(static_cast<lv_obj_t*>(var), (lv_opa_t)v, 0);
}

namespace {
static void anim_set_obj_opa(void* var, int32_t v)
{
    lv_obj_set_style_opa(static_cast<lv_obj_t*>(var), (lv_opa_t)v, 0);
}

static void animate_fade(lv_obj_t* obj, lv_opa_t from, lv_opa_t to, uint16_t ms, uint16_t delay_ms)
{
    if(!obj) return;
    lv_anim_del(obj, anim_set_obj_opa);
    lv_obj_set_style_opa(obj, from, 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, anim_set_obj_opa);
    lv_anim_set_time(&a, ms);
    lv_anim_set_delay(&a, delay_ms);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_values(&a, (int32_t)from, (int32_t)to);
    lv_anim_start(&a);
}
} // namespace

void ShutdownPage::ensure_built(lv_obj_t* parent)
{
    if(built_ || !parent) return;
    built_ = true;

    root_ = lv_obj_create(parent);
    lv_obj_remove_style_all(root_);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(root_, lv_color_black(), 0);
    lv_obj_set_style_border_width(root_, 0, 0);
    lv_obj_clear_flag(root_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);

    arc_ = lv_arc_create(root_);
    lv_obj_set_size(arc_, 118, 118);
    lv_obj_align(arc_, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_rotation(arc_, 270);
    lv_arc_set_bg_angles(arc_, 0, 360);
    lv_arc_set_range(arc_, 0, 100);
    lv_arc_set_value(arc_, 0);
    lv_obj_remove_style(arc_, nullptr, LV_PART_KNOB);
    lv_obj_set_style_arc_width(arc_, 8, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_, core::theme::primary(), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_, 8, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_, core::theme::divider(), LV_PART_MAIN);
    lv_obj_set_style_arc_opa(arc_, LV_OPA_40, LV_PART_MAIN);

    title_ = lv_label_create(root_);
    lv_label_set_text(title_, "SHUTTING DOWN");
    lv_obj_set_style_text_color(title_, core::theme::secondary(), 0);
    lv_obj_set_style_text_letter_space(title_, 3, 0);
    lv_obj_align_to(title_, arc_, LV_ALIGN_OUT_TOP_MID, 0, -22);

    subtitle_ = lv_label_create(root_);
    lv_label_set_text(subtitle_, "PLEASE WAIT");
    lv_obj_set_style_text_color(subtitle_, core::theme::secondary(), 0);
    lv_obj_set_style_text_opa(subtitle_, LV_OPA_80, 0);
    lv_obj_set_style_text_letter_space(subtitle_, 2, 0);
    lv_obj_align_to(subtitle_, arc_, LV_ALIGN_OUT_BOTTOM_MID, 0, 18);
}

void ShutdownPage::on_show()
{
    requested_ = false;
    start_ms_ = lv_tick_get();
    last_text_ms_ = start_ms_;
    shutdown_ms_ = 0;
    dots_ = 0;

    if(root_) {
        lv_anim_del(root_, anim_set_root_opa);
        lv_obj_set_style_opa(root_, 255, 0);
    }

    // Text: gradual appear, slight stagger.
    if(title_) {
        animate_fade(title_, 0, 255, (uint16_t)SUNNY_UI_PWR_TEXT_FADE_MS, 0);
    }
    if(subtitle_) {
        animate_fade(subtitle_, 0, 255, (uint16_t)SUNNY_UI_PWR_TEXT_FADE_MS, (uint16_t)SUNNY_UI_PWR_TEXT_STAGGER_MS);
    }

    if(!arc_) return;
    lv_anim_del(arc_, anim_set_arc_value);
    lv_arc_set_value(arc_, 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, arc_);
    lv_anim_set_exec_cb(&a, anim_set_arc_value);
    lv_anim_set_time(&a, SUNNY_UI_SHUTDOWN_ANIM_MS);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_start(&a);
}

void ShutdownPage::on_hide()
{
    if(arc_) lv_anim_del(arc_, anim_set_arc_value);
    if(root_) lv_anim_del(root_, anim_set_root_opa);
    if(title_) lv_anim_del(title_, anim_set_obj_opa);
    if(subtitle_) lv_anim_del(subtitle_, anim_set_obj_opa);
}

void ShutdownPage::update_loading_text(std::uint32_t now_ms)
{
    if(!subtitle_) return;
    if(now_ms - last_text_ms_ < 220) return;
    last_text_ms_ = now_ms;

    dots_ = (std::uint8_t)((dots_ + 1) % 4);
    char buf[20]{};
    std::size_t n = 0;
    const char* base = "PLEASE WAIT";
    while(base[n] != '\0' && n < sizeof(buf) - 1) {
        buf[n] = base[n];
        n++;
    }
    for(std::uint8_t i = 0; i < dots_ && n < sizeof(buf) - 1; i++) {
        buf[n++] = '.';
    }
    buf[n] = '\0';
    lv_label_set_text(subtitle_, buf);
}

void ShutdownPage::on_update()
{
    const std::uint32_t now = lv_tick_get();
    update_loading_text(now);

    if(requested_) {
        if(shutdown_ms_ != 0 && (int32_t)(now - shutdown_ms_) < 0) {
            // Still waiting for the fade-out to finish (wrap-safe).
            return;
        }
        vsun::port::request_shutdown();
        return;
    }
    const std::uint32_t elapsed = now - start_ms_;
    if(elapsed >= (std::uint32_t)SUNNY_UI_SHUTDOWN_ANIM_MS) {
        requested_ = true;
        shutdown_ms_ = now + 220;

        // Fade out before exiting to mimic "screen off".
        if(title_) {
            animate_fade(title_, 255, 0, (uint16_t)SUNNY_UI_PWR_TEXT_FADE_MS, 0);
        }
        if(subtitle_) {
            animate_fade(subtitle_, 255, 0, (uint16_t)SUNNY_UI_PWR_TEXT_FADE_MS, 0);
        }
        if(root_) {
            lv_anim_del(root_, anim_set_root_opa);
            lv_anim_t ao;
            lv_anim_init(&ao);
            lv_anim_set_var(&ao, root_);
            lv_anim_set_exec_cb(&ao, anim_set_root_opa);
            lv_anim_set_time(&ao, 220);
            lv_anim_set_path_cb(&ao, lv_anim_path_ease_in);
            lv_anim_set_values(&ao, 255, 0);
            lv_anim_start(&ao);
        }
    }
}

} // namespace vsun::pages
