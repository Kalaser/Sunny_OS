#pragma once

#include "apps/placeholder/placeholder_slice_base.h"
#include "sunny_resources.h"

namespace vsun::apps::music {

class MusicSlice final : public placeholder::PlaceholderSliceBase {
public:
    MusicSlice() : PlaceholderSliceBase("music", "MUSIC", spec()) {}

private:
    static placeholder::PlaceholderSpec spec()
    {
        return placeholder::PlaceholderSpec{
            "MUSIC",
            SUNNY_IMG(MYNAUI_MUSIC_SOLID),
            "NOW PLAYING: Neon Sunset",
            "VOL 68%",
            "SHUFFLE",
            "LOSSLESS",
            54};
    }
};

} // namespace vsun::apps::music
