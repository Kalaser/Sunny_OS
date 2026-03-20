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
    MusicSlice() : PlaceholderSliceBase("music", "MUSIC", build_spec()) {}

private:
    static placeholder::PlaceholderSpec build_spec()
    {
        static const placeholder::PlaceholderItem kItems[] = {
            {SUNNY_IMG(MYNAUI_MUSIC_SOLID), "Artist", "A. Morgan"},
            {SUNNY_IMG(MYNAUI_ALARM_SOLID), "Remaining", "02:14"},
            {SUNNY_IMG(MYNAUI_HEADPHONES_SOLID), "Output", "Speaker"},
        };
        return placeholder::PlaceholderSpec{
            "MUSIC",
            SUNNY_IMG(MYNAUI_MUSIC_SOLID),
            "NOW PLAYING: Neon Sunset",
            "VOL 68%",
            "SHUFFLE",
            "LOSSLESS",
            54};
    }
            54,
            kItems,
            static_cast<std::uint8_t>(sizeof(kItems) / sizeof(kItems[0]))};
    }
    MusicSlice()
        : PlaceholderSliceBase(
              "music",
              "MUSIC",
              placeholder::PlaceholderSpec{
                  "MUSIC",
                  SUNNY_IMG(MYNAUI_MUSIC_SOLID),
                  "NOW PLAYING: Neon Sunset",
                  "VOL 68%",
                  "SHUFFLE",
                  "LOSSLESS",
                  54}) {}
};

} // namespace vsun::apps::music
