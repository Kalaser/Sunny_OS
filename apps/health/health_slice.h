#pragma once

#include "apps/placeholder/placeholder_slice_base.h"
#include "sunny_resources.h"

namespace vsun::apps::health {

class HealthSlice final : public placeholder::PlaceholderSliceBase {
public:
    HealthSlice() : PlaceholderSliceBase("health", "HEALTH", spec()) {}

private:
    static placeholder::PlaceholderSpec spec()
    {
        return placeholder::PlaceholderSpec{
            "HEALTH",
            SUNNY_IMG(MYNAUI_HEART_SOLID),
            "HEART RATE 72 BPM",
            "GOAL 6,400 / 8,000",
            "STEPS",
            "SLEEP 7H",
            80};
    }
};

} // namespace vsun::apps::health
