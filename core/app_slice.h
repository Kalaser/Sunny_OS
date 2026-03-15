#pragma once

#include "lvgl.h"

namespace vsun::core {

// AppSlice is the application module interface in SUNNY_OS.
// - Each app can be implemented as one slice.
// - The slice owns lazy UI construction via ensure_built() to keep boot fast.
class AppSlice {
public:
    virtual ~AppSlice() = default;

    virtual const char* id() const = 0;
    virtual const char* label() const = 0;

    // Lazy build: create UI only when it is first shown.
    virtual void ensure_built(lv_obj_t* parent) = 0;

    // Optional callbacks on navigation.
    virtual void on_show() {}
    virtual void on_hide() {}

    virtual lv_obj_t* root() const = 0;
};

} // namespace vsun::core
