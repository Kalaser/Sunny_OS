#pragma once

#include "apps/placeholder/placeholder_slice_base.h"
#include "sunny_resources.h"

namespace vsun::apps::radio {

class RadioSlice final : public placeholder::PlaceholderSliceBase {
public:
    RadioSlice() : PlaceholderSliceBase("radio", "RADIO", spec()) {}

private:
    static placeholder::PlaceholderSpec spec()
    {
    RadioSlice() : PlaceholderSliceBase("radio", "RADIO", build_spec()) {}

private:
    static placeholder::PlaceholderSpec build_spec()
    {
        static const placeholder::PlaceholderItem kItems[] = {
            {SUNNY_IMG(MYNAUI_RADIO_SOLID), "Station", "Sunny FM"},
            {SUNNY_IMG(MYNAUI_SIGNAL_SOLID), "Strength", "-57 dBm"},
            {SUNNY_IMG(MYNAUI_PLAY_SOLID), "Mode", "Live"},
        };
        return placeholder::PlaceholderSpec{
            "RADIO",
            SUNNY_IMG(MYNAUI_WIFI_MEDIUM_SOLID),
            "CONNECTED TO SUNNY FM",
            "SIGNAL STABLE",
            "98.3 MHz",
            "STEREO",
            91};
    }
            91,
            kItems,
            static_cast<std::uint8_t>(sizeof(kItems) / sizeof(kItems[0]))};
    }
    RadioSlice()
        : PlaceholderSliceBase(
              "radio",
              "RADIO",
              placeholder::PlaceholderSpec{
                  "RADIO",
                  SUNNY_IMG(MYNAUI_WIFI_MEDIUM_SOLID),
                  "CONNECTED TO SUNNY FM",
                  "SIGNAL STABLE",
                  "98.3 MHz",
                  "STEREO",
                  91}) {}
};

} // namespace vsun::apps::radio
