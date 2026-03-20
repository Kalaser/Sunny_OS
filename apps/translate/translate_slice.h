#pragma once

#include "apps/placeholder/placeholder_slice_base.h"
#include "sunny_resources.h"

namespace vsun::apps::translate {

class TranslateSlice final : public placeholder::PlaceholderSliceBase {
public:
    TranslateSlice() : PlaceholderSliceBase("translate", "TRANSLATE", build_spec()) {}

private:
    static placeholder::PlaceholderSpec build_spec()
    {
        static const placeholder::PlaceholderItem kItems[] = {
            {SUNNY_IMG(MYNAUI_MICROPHONE_SOLID), "Input", "Voice"},
            {SUNNY_IMG(MYNAUI_REFRESH_SOLID), "Target", "中文"},
            {SUNNY_IMG(MYNAUI_ALARM_SOLID), "Latency", "120ms"},
        };
        return placeholder::PlaceholderSpec{
            "TRANSLATE",
            SUNNY_IMG(MYNAUI_REFRESH_SOLID),
            "REAL-TIME LANGUAGE MODE",
            "EN <-> 中文",
            "VOICE ON",
            "TEXT LOCK",
            72,
            kItems,
            static_cast<std::uint8_t>(sizeof(kItems) / sizeof(kItems[0]))};
    }
};

} // namespace vsun::apps::translate
