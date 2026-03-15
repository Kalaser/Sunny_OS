#pragma once

#include "apps/placeholder/placeholder_slice_base.h"

namespace vsun::apps::radio {

class RadioSlice final : public placeholder::PlaceholderSliceBase {
public:
    RadioSlice() : PlaceholderSliceBase("radio", "RADIO") {}
};

} // namespace vsun::apps::radio
