#pragma once

#include <string>

#include "lvgl.h"

#include "services/datacenter.h"

namespace vsun::core {

class StatusBarView {
public:
    void build(lv_obj_t* parent);
    void set_time_text(const char* hhmm);
    void set_battery_text(const char* percent);
    lv_obj_t* root() const { return root_; }

private:
    static void on_data(vsun::services::Topic topic, const void* data, size_t len, void* user);
    static void on_root_event(lv_event_t* e);

    void set_wifi_state(vsun::services::LinkState s);
    void set_ble_state(vsun::services::LinkState s);
    void set_battery_percent(std::uint8_t percent);

    lv_obj_t* root_ = nullptr;
    lv_obj_t* time_ = nullptr;
    lv_obj_t* right_ = nullptr;
    lv_obj_t* wifi_ = nullptr;      // lv_img
    lv_obj_t* ble_ = nullptr;       // lv_img
    lv_obj_t* batt_wrap_ = nullptr; // flex row
    lv_obj_t* batt_icon_ = nullptr; // lv_img
    lv_obj_t* batt_text_ = nullptr; // lv_label

    vsun::services::DataCenter::Subscription sub_time_{};
    vsun::services::DataCenter::Subscription sub_batt_{};
    vsun::services::DataCenter::Subscription sub_wifi_{};
    vsun::services::DataCenter::Subscription sub_ble_{};
};

} // namespace vsun::core
