#pragma once

#include "lvgl.h"

namespace vsun::apps::maps {

class MapsView {
public:
    void build(lv_obj_t* parent);
    void set_distance(int meters);
    lv_obj_t* root() const { return root_; }

private:
    lv_obj_t* root_ = nullptr;
    lv_obj_t* distance_ = nullptr;
};

} // namespace vsun::apps::maps
