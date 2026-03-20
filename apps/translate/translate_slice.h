#pragma once

#include "apps/placeholder/placeholder_slice_base.h"
#include "sunny_resources.h"

namespace vsun::apps::translate {

class TranslateSlice final : public placeholder::PlaceholderSliceBase {
public:
    TranslateSlice() : PlaceholderSliceBase("translate", "TRANSLATE", spec()) {}

private:
    static placeholder::PlaceholderSpec spec()
    {
        return placeholder::PlaceholderSpec{
            "TRANSLATE",
            SUNNY_IMG(MYNAUI_REFRESH_SOLID),
            "REAL-TIME LANGUAGE MODE",
            "EN <-> 中文",
            "VOICE ON",
            "TEXT LOCK",
            72};
    }
};

} // namespace vsun::apps::translate
