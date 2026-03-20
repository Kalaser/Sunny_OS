#pragma once

#include "apps/placeholder/placeholder_slice_base.h"
#include "sunny_resources.h"

namespace vsun::apps::notes {

class NotesSlice final : public placeholder::PlaceholderSliceBase {
public:
    NotesSlice() : PlaceholderSliceBase("notes", "NOTES", spec()) {}

private:
    static placeholder::PlaceholderSpec spec()
    {
        return placeholder::PlaceholderSpec{
            "NOTES",
            SUNNY_IMG(MYNAUI_EDIT_SOLID),
            "3 tasks due today",
            "SYNCED 2 MIN AGO",
            "TODO",
            "DRAFT",
            38};
    }
};

} // namespace vsun::apps::notes
