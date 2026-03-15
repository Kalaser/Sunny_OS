#include "apps/ai/ai_slice.h"

namespace vsun::apps::ai {

void AiSlice::ensure_built(lv_obj_t* parent)
{
    if(built_) return;
    (void)model_;
    view_.build(parent);
    built_ = true;
}

} // namespace vsun::apps::ai
