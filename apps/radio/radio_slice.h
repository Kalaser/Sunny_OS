#pragma once

#include "apps/placeholder/placeholder_slice_base.h"
#include "sunny_resources.h"

namespace vsun::apps::radio {

class RadioSlice final : public placeholder::PlaceholderSliceBase {
public:
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
