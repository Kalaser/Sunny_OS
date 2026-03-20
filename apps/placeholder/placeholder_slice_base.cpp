#include "apps/placeholder/placeholder_slice_base.h"

namespace vsun::apps::placeholder {

void PlaceholderSliceBase::ensure_built(lv_obj_t* parent)
{
    if(built_) return;
    (void)model_;
    view_.build(parent, spec_);
    built_ = true;
}

} // namespace vsun::apps::placeholder
