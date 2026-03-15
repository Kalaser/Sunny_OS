#pragma once

#include "apps/placeholder/placeholder_slice_base.h"

namespace vsun::apps::music {

class MusicSlice final : public placeholder::PlaceholderSliceBase {
public:
    MusicSlice() : PlaceholderSliceBase("music", "MUSIC") {}
};

} // namespace vsun::apps::music
