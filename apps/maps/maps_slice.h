#pragma once

#include "core/app_slice.h"
#include "apps/maps/maps_model.h"
#include "apps/maps/maps_view.h"

namespace vsun::apps::maps {

class MapsSlice final : public core::AppSlice {
public:
    const char* id() const override { return "maps"; }
    const char* label() const override { return "MAPS"; }

    void ensure_built(lv_obj_t* parent) override;
    lv_obj_t* root() const override { return view_.root(); }

private:
    MapsModel model_{};
    MapsView view_{};
    bool built_ = false;
};

} // namespace vsun::apps::maps
