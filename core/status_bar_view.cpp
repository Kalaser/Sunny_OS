#include "core/status_bar_view.h"

#include <cstdio>

#include "config/ui_config.h"
#include "core/theme.h"
#include "sunny_resources.h"

namespace vsun::core {
namespace {

static void icon_opa_anim_cb(void* var, int32_t v)
{
    lv_obj_set_style_opa(static_cast<lv_obj_t*>(var), (lv_opa_t)v, 0);
}

static void apply_link_style(lv_obj_t* obj, vsun::services::LinkState s)
{
    if(!obj) return;

    // Remove any previous blink animation.
    lv_anim_del(obj, icon_opa_anim_cb);

    // Keep icons visible but vary color/opacity by link state.
    if(s == vsun::services::LinkState::Up) {
        lv_obj_set_style_img_recolor(obj, theme::primary(), 0);
        lv_obj_set_style_img_recolor_opa(obj, LV_OPA_COVER, 0);
        lv_obj_set_style_opa(obj, LV_OPA_COVER, 0);
        return;
    }

    if(s == vsun::services::LinkState::Connecting) {
        lv_obj_set_style_img_recolor(obj, theme::secondary(), 0);
        lv_obj_set_style_img_recolor_opa(obj, LV_OPA_COVER, 0);
        // Blink while connecting.
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);
        lv_anim_set_exec_cb(&a, icon_opa_anim_cb);
        lv_anim_set_values(&a, LV_OPA_40, LV_OPA_COVER);
        lv_anim_set_time(&a, 600);
        lv_anim_set_playback_time(&a, 600);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
        lv_anim_start(&a);
        return;
    }

    // Down / unknown: dim it.
    lv_obj_set_style_img_recolor(obj, theme::divider(), 0);
    lv_obj_set_style_img_recolor_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_opa(obj, LV_OPA_70, 0);
}

static const lv_img_dsc_t* battery_img_for(uint8_t percent)
{
    if(percent >= 90) return SUNNY_IMG(MYNAUI_BATTERY_CHARGING_FOUR_SOLID);
    if(percent >= 70) return SUNNY_IMG(MYNAUI_BATTERY_CHARGING_THREE_SOLID);
    if(percent >= 40) return SUNNY_IMG(MYNAUI_BATTERY_CHARGING_TWO_SOLID);
    if(percent >= 15) return SUNNY_IMG(MYNAUI_BATTERY_CHARGING_ONE_SOLID);
    return SUNNY_IMG(MYNAUI_BATTERY_EMPTY_SOLID);
}

} // namespace

void StatusBarView::on_data(vsun::services::Topic topic, const void* data, size_t len, void* user)
{
    auto* self = static_cast<StatusBarView*>(user);
    if(!self) return;

    if(topic == vsun::services::Topic::TimeHHMM) {
        if(len != sizeof(vsun::services::TimeHHMMPayload)) return;
        const auto* p = static_cast<const vsun::services::TimeHHMMPayload*>(data);
        self->set_time_text(p->text);
        return;
    }

    if(topic == vsun::services::Topic::BatteryPercent) {
        if(len != sizeof(vsun::services::BatteryPercentPayload)) return;
        const auto* p = static_cast<const vsun::services::BatteryPercentPayload*>(data);
        self->set_battery_percent(p->percent);
        return;
    }

    if(topic == vsun::services::Topic::WifiState) {
        if(len != sizeof(vsun::services::WifiStatePayload)) return;
        const auto* p = static_cast<const vsun::services::WifiStatePayload*>(data);
        self->set_wifi_state(p->state);
        return;
    }

    if(topic == vsun::services::Topic::BleState) {
        if(len != sizeof(vsun::services::BleStatePayload)) return;
        const auto* p = static_cast<const vsun::services::BleStatePayload*>(data);
        self->set_ble_state(p->state);
        return;
    }
}

void StatusBarView::on_root_event(lv_event_t* e)
{
    if(!e) return;
    if(lv_event_get_code(e) != LV_EVENT_DELETE) return;
    auto* self = static_cast<StatusBarView*>(lv_event_get_user_data(e));
    if(!self) return;

    auto& dc = vsun::services::DataCenter::instance();
    dc.unsubscribe(self->sub_time_);
    dc.unsubscribe(self->sub_batt_);
    dc.unsubscribe(self->sub_wifi_);
    dc.unsubscribe(self->sub_ble_);
    self->sub_time_ = {};
    self->sub_batt_ = {};
    self->sub_wifi_ = {};
    self->sub_ble_ = {};
}

void StatusBarView::build(lv_obj_t* parent)
{
    root_ = lv_obj_create(parent);
    lv_obj_remove_style_all(root_);
    lv_obj_set_style_bg_opa(root_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(root_, 0, 0);
    lv_obj_set_style_pad_left(root_, 12, 0);
    lv_obj_set_style_pad_right(root_, 12, 0);
    lv_obj_set_style_pad_top(root_, 0, 0);
    lv_obj_set_style_pad_bottom(root_, 0, 0);

    lv_obj_set_size(root_, lv_pct(100), SUNNY_UI_STATUS_BAR_H);
    lv_obj_align(root_, LV_ALIGN_TOP_MID, 0, 0);

    time_ = lv_label_create(root_);
    lv_label_set_text(time_, "00:00");
    lv_obj_set_style_text_color(time_, theme::secondary(), 0);
    lv_obj_align(time_, LV_ALIGN_LEFT_MID, 0, 0);

    // Right side: WiFi / BLE icons close to battery.
    right_ = lv_obj_create(root_);
    lv_obj_remove_style_all(right_);
    lv_obj_set_size(right_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(right_, 0, 0);
    lv_obj_set_style_bg_opa(right_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(right_, 0, 0);
    lv_obj_set_flex_flow(right_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(right_, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(right_, 8, 0);
    lv_obj_align(right_, LV_ALIGN_RIGHT_MID, 0, 0);

    wifi_ = lv_img_create(right_);
    lv_img_set_src(wifi_, SUNNY_IMG(MYNAUI_WIFI_MINUS_SOLID));
    lv_obj_set_style_img_recolor(wifi_, theme::divider(), 0);
    lv_obj_set_style_img_recolor_opa(wifi_, LV_OPA_COVER, 0);
    lv_obj_set_style_opa(wifi_, LV_OPA_70, 0);

    ble_ = lv_img_create(right_);
    lv_img_set_src(ble_, SUNNY_IMG(MYNAUI_BLUETOOTH_SOLID));
    lv_obj_set_style_img_recolor(ble_, theme::divider(), 0);
    lv_obj_set_style_img_recolor_opa(ble_, LV_OPA_COVER, 0);
    lv_obj_set_style_opa(ble_, LV_OPA_70, 0);

    batt_wrap_ = lv_obj_create(right_);
    lv_obj_remove_style_all(batt_wrap_);
    lv_obj_set_size(batt_wrap_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(batt_wrap_, 0, 0);
    lv_obj_set_style_bg_opa(batt_wrap_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(batt_wrap_, 0, 0);
    lv_obj_set_flex_flow(batt_wrap_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(batt_wrap_, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(batt_wrap_, 4, 0);

    batt_icon_ = lv_img_create(batt_wrap_);
    lv_img_set_src(batt_icon_, SUNNY_IMG(MYNAUI_BATTERY_CHARGING_FOUR_SOLID));
    lv_obj_set_style_img_recolor(batt_icon_, theme::secondary(), 0);
    lv_obj_set_style_img_recolor_opa(batt_icon_, LV_OPA_COVER, 0);

    batt_text_ = lv_label_create(batt_wrap_);
    lv_label_set_text(batt_text_, "95%");
    lv_obj_set_style_text_color(batt_text_, theme::secondary(), 0);

    // Subscribe system status (time/battery/connectivity).
    auto& dc = vsun::services::DataCenter::instance();
    sub_time_ = dc.subscribe(vsun::services::Topic::TimeHHMM, &StatusBarView::on_data, this);
    sub_batt_ = dc.subscribe(vsun::services::Topic::BatteryPercent, &StatusBarView::on_data, this);
    sub_wifi_ = dc.subscribe(vsun::services::Topic::WifiState, &StatusBarView::on_data, this);
    sub_ble_ = dc.subscribe(vsun::services::Topic::BleState, &StatusBarView::on_data, this);
    lv_obj_add_event_cb(root_, on_root_event, LV_EVENT_DELETE, this);

    lv_obj_t* divider = lv_obj_create(parent);
    lv_obj_remove_style_all(divider);
    lv_obj_set_size(divider, lv_pct(100), 1);
    lv_obj_set_style_bg_opa(divider, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(divider, theme::divider(), 0);
    lv_obj_set_style_border_width(divider, 0, 0);
    lv_obj_align(divider, LV_ALIGN_TOP_MID, 0, SUNNY_UI_STATUS_BAR_H - 1);
}

void StatusBarView::set_time_text(const char* hhmm)
{
    if(time_) lv_label_set_text(time_, hhmm ? hhmm : "");
}

void StatusBarView::set_battery_text(const char* percent)
{
    if(batt_text_) lv_label_set_text(batt_text_, percent ? percent : "");
}

void StatusBarView::set_wifi_state(vsun::services::LinkState s)
{
    if(wifi_) {
        const lv_img_dsc_t* src = SUNNY_IMG(MYNAUI_WIFI_MINUS_SOLID);
        if(s == vsun::services::LinkState::Up) src = SUNNY_IMG(MYNAUI_WIFI_MEDIUM_SOLID);
        else if(s == vsun::services::LinkState::Connecting) src = SUNNY_IMG(MYNAUI_WIFI_LOW_SOLID);
        lv_img_set_src(wifi_, src);
    }
    apply_link_style(wifi_, s);
}

void StatusBarView::set_ble_state(vsun::services::LinkState s)
{
    apply_link_style(ble_, s);
}

void StatusBarView::set_battery_percent(std::uint8_t percent)
{
    if(!batt_icon_ || !batt_text_) return;
    if(percent > 100) percent = 100;
    char buf[32]{};
    std::snprintf(buf, sizeof(buf), "%u%%", (unsigned)percent);
    lv_label_set_text(batt_text_, buf);
    lv_img_set_src(batt_icon_, battery_img_for(percent));
}

} // namespace vsun::core
