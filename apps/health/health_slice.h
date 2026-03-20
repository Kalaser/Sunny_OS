#pragma once

#include "apps/placeholder/placeholder_slice_base.h"
#include "sunny_resources.h"

namespace vsun::apps::health {

class HealthSlice final : public placeholder::PlaceholderSliceBase {
public:
    HealthSlice() : PlaceholderSliceBase("health", "HEALTH", build_spec()) {}

private:
    static placeholder::PlaceholderSpec build_spec()
    {
        static const placeholder::PlaceholderItem kItems[] = {
            {SUNNY_IMG(MYNAUI_HEART_SOLID), "Heart", "72 bpm"},
            {SUNNY_IMG(MYNAUI_ACTIVITY_SOLID), "Steps", "6400"},
            {SUNNY_IMG(MYNAUI_MOON_SOLID), "Sleep", "7h 04m"},
        };
        return placeholder::PlaceholderSpec{
            "HEALTH",
            SUNNY_IMG(MYNAUI_HEART_SOLID),
            "HEART RATE 72 BPM",
            "GOAL 6,400 / 8,000",
            "STEPS",
            "SLEEP 7H",
            80,
            kItems,
            static_cast<std::uint8_t>(sizeof(kItems) / sizeof(kItems[0]))};
    }
};

} // namespace vsun::apps::health
