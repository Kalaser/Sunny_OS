#pragma once

#include "apps/placeholder/placeholder_slice_base.h"

namespace vsun::apps::notes {

class NotesSlice final : public placeholder::PlaceholderSliceBase {
public:
    NotesSlice() : PlaceholderSliceBase("notes", "NOTES") {}
};

} // namespace vsun::apps::notes
