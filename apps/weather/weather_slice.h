#pragma once

#include "apps/placeholder/placeholder_slice_base.h"

namespace vsun::apps::weather {

class WeatherSlice final : public placeholder::PlaceholderSliceBase {
public:
    WeatherSlice() : PlaceholderSliceBase("weather", "WEATHER") {}
};

} // namespace vsun::apps::weather
