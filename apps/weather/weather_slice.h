#pragma once

#include "apps/placeholder/placeholder_slice_base.h"
#include "sunny_resources.h"

namespace vsun::apps::weather {

class WeatherSlice final : public placeholder::PlaceholderSliceBase {
public:
    WeatherSlice()
        : PlaceholderSliceBase(
              "weather",
              "WEATHER",
              placeholder::PlaceholderSpec{
                  "WEATHER",
                  SUNNY_IMG(MYNAUI_CLOUD_SUN_SOLID),
                  "PARTLY CLOUDY · 23°C",
                  "AQI 42 GOOD",
                  "H:27°",
                  "L:18°",
                  64}) {}
};

} // namespace vsun::apps::weather
