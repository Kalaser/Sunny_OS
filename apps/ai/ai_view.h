#pragma once

#include "lvgl.h"

namespace vsun::apps::ai {

class AiView {
public:
    void build(lv_obj_t* parent);
    lv_obj_t* root() const { return root_; }

private:
    lv_obj_t* root_ = nullptr;
};

} // namespace vsun::apps::ai
