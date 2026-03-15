#pragma once

#include "lvgl.h"

namespace vsun::apps::camera {

class CameraView {
public:
    void build(lv_obj_t* parent, lv_event_cb_t on_close, void* close_user);
    lv_obj_t* root() const { return root_; }

private:
    lv_obj_t* root_ = nullptr;
};

} // namespace vsun::apps::camera
