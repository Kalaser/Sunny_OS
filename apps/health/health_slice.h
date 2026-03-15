#pragma once

#include "apps/placeholder/placeholder_slice_base.h"

namespace vsun::apps::health {

class HealthSlice final : public placeholder::PlaceholderSliceBase {
public:
    HealthSlice() : PlaceholderSliceBase("health", "HEALTH") {}
};

} // namespace vsun::apps::health
