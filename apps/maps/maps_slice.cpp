#include "apps/maps/maps_slice.h"

namespace vsun::apps::maps {

void MapsSlice::ensure_built(lv_obj_t* parent)
{
    if(built_) return;
    view_.build(parent);
    view_.set_distance(model_.distance_m);
    built_ = true;
}

} // namespace vsun::apps::maps
