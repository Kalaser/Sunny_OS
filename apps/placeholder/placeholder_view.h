#pragma once

#include "lvgl.h"

namespace vsun::apps::placeholder {

class PlaceholderView {
public:
    void build(lv_obj_t* parent, const char* title);
    lv_obj_t* root() const { return root_; }

private:
    lv_obj_t* root_ = nullptr;
};

} // namespace vsun::apps::placeholder
