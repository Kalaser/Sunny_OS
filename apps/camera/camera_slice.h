#pragma once

#include "core/app_slice.h"
#include "apps/camera/camera_model.h"
#include "apps/camera/camera_view.h"

namespace vsun::apps::camera {

class CameraSlice final : public core::AppSlice {
public:
    const char* id() const override { return "camera"; }
    const char* label() const override { return "RECORDER"; }

    void ensure_built(lv_obj_t* parent) override;
    lv_obj_t* root() const override { return view_.root(); }

    void set_on_close(lv_event_cb_t cb, void* user) { on_close_ = cb; close_user_ = user; }

private:
    CameraModel model_{};
    CameraView view_{};
    bool built_ = false;
    lv_event_cb_t on_close_ = nullptr;
    void* close_user_ = nullptr;
};

} // namespace vsun::apps::camera
