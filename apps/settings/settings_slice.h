#pragma once

#include "apps/placeholder/placeholder_slice_base.h"
#include "sunny_resources.h"

namespace vsun::apps::settings {

class SettingsSlice final : public placeholder::PlaceholderSliceBase {
public:
    SettingsSlice()
        : PlaceholderSliceBase(
              "settings",
              "SETTINGS",
              placeholder::PlaceholderSpec{
                  "SETTINGS",
                  SUNNY_IMG(MYNAUI_COG_SOLID),
                  "SYSTEM READY FOR UPDATE",
                  "BATTERY SAVER OFF",
                  "WIFI",
                  "DISPLAY",
                  46}) {}
};

} // namespace vsun::apps::settings
