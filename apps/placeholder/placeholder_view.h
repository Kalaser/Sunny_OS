#pragma once

#include <cstdint>

#include "lvgl.h"

namespace vsun::apps::placeholder {

struct PlaceholderItem {
    const void* icon = nullptr;
    const char* key = nullptr;
    const char* value = nullptr;
};

struct PlaceholderSpec {
    const char* title = "APP";
    const void* icon = nullptr;
    const char* subtitle = "APPLICATION LOADED";
    const char* detail = "READY";
    const char* chip_left = nullptr;
    const char* chip_right = nullptr;
    int progress = -1; // [0,100], <0 means hide progress

    const PlaceholderItem* items = nullptr;
    std::uint8_t item_count = 0;
};

class PlaceholderView {
public:
    void build(lv_obj_t* parent, const PlaceholderSpec& spec);
    lv_obj_t* root() const { return root_; }

private:
    lv_obj_t* root_ = nullptr;
};

} // namespace vsun::apps::placeholder
