#pragma once

#include "apps/placeholder/placeholder_slice_base.h"

namespace vsun::apps::translate {

class TranslateSlice final : public placeholder::PlaceholderSliceBase {
public:
    TranslateSlice() : PlaceholderSliceBase("translate", "TRANSLATE") {}
};

} // namespace vsun::apps::translate
