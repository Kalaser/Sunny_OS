#pragma once

#include "apps/placeholder/placeholder_slice_base.h"
#include "sunny_resources.h"

namespace vsun::apps::weather {

class WeatherSlice final : public placeholder::PlaceholderSliceBase {
public:
    WeatherSlice() : PlaceholderSliceBase("weather", "WEATHER", spec()) {}

private:
    static placeholder::PlaceholderSpec spec()
    {
    WeatherSlice() : PlaceholderSliceBase("weather", "WEATHER", build_spec()) {}

private:
    static placeholder::PlaceholderSpec build_spec()
    {
        static const placeholder::PlaceholderItem kItems[] = {
            {SUNNY_IMG(MYNAUI_WIND_SOLID), "Wind", "3.8m/s"},
            {SUNNY_IMG(MYNAUI_DROPLET_SOLID), "Humidity", "56%"},
            {SUNNY_IMG(MYNAUI_SUN_SOLID), "UV Index", "Moderate"},
        };
        return placeholder::PlaceholderSpec{
            "WEATHER",
            SUNNY_IMG(MYNAUI_CLOUD_SUN_SOLID),
            "PARTLY CLOUDY · 23°C",
            "AQI 42 GOOD",
            "H:27°",
            "L:18°",
            64};
    }
            64,
            kItems,
            static_cast<std::uint8_t>(sizeof(kItems) / sizeof(kItems[0]))};
    }
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
