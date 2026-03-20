#pragma once

#include "apps/placeholder/placeholder_slice_base.h"
#include "sunny_resources.h"

namespace vsun::apps::settings {

class SettingsSlice final : public placeholder::PlaceholderSliceBase {
public:
    SettingsSlice() : PlaceholderSliceBase("settings", "SETTINGS", build_spec()) {}

private:
    static placeholder::PlaceholderSpec build_spec()
    {
        static const placeholder::PlaceholderItem kItems[] = {
            {SUNNY_IMG(MYNAUI_SUN_SOLID), "Brightness", "75%"},
            {SUNNY_IMG(MYNAUI_SPEAKER_SOLID), "Volume", "50%"},
            {SUNNY_IMG(MYNAUI_WIFI_SOLID), "Wi-Fi", "On"},
            {SUNNY_IMG(MYNAUI_BLUETOOTH_SOLID), "Bluetooth", "Off"},
        };
        return placeholder::PlaceholderSpec{
            "SETTINGS",
            SUNNY_IMG(MYNAUI_COG_SOLID),
            "SYSTEM READY FOR UPDATE",
            "BATTERY SAVER OFF",
            "WIFI",
            "DISPLAY",
            46,
            kItems,
            static_cast<std::uint8_t>(sizeof(kItems) / sizeof(kItems[0]))};
    }
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
