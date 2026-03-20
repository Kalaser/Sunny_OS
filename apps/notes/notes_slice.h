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
    NotesSlice() : PlaceholderSliceBase("notes", "NOTES", build_spec()) {}

private:
    static placeholder::PlaceholderSpec build_spec()
    {
        static const placeholder::PlaceholderItem kItems[] = {
            {SUNNY_IMG(MYNAUI_CHECK_SOLID), "Inbox", "5 notes"},
            {SUNNY_IMG(MYNAUI_CALENDAR_SOLID), "Today", "3 tasks"},
            {SUNNY_IMG(MYNAUI_CLOUD_SOLID), "Sync", "Online"},
        };
        return placeholder::PlaceholderSpec{
            "NOTES",
            SUNNY_IMG(MYNAUI_EDIT_SOLID),
            "3 TASKS DUE TODAY",
            "SYNCED 2 MIN AGO",
            "TODO",
            "DRAFT",
            38,
            kItems,
            static_cast<std::uint8_t>(sizeof(kItems) / sizeof(kItems[0]))};
    }
    NotesSlice()
        : PlaceholderSliceBase(
              "notes",
              "NOTES",
              placeholder::PlaceholderSpec{
                  "NOTES",
                  SUNNY_IMG(MYNAUI_EDIT_SOLID),
                  "3 tasks due today",
                  "SYNCED 2 MIN AGO",
                  "TODO",
                  "DRAFT",
                  38}) {}
};

} // namespace vsun::apps::notes
