#include "apps/camera/camera_slice.h"

namespace vsun::apps::camera {

void CameraSlice::ensure_built(lv_obj_t* parent)
{
    if(built_) return;
    (void)model_;
    view_.build(parent, on_close_, close_user_);
    built_ = true;
}

} // namespace vsun::apps::camera
