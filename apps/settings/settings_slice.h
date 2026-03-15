#pragma once

#include "apps/placeholder/placeholder_slice_base.h"

namespace vsun::apps::settings {

class SettingsSlice final : public placeholder::PlaceholderSliceBase {
public:
    SettingsSlice() : PlaceholderSliceBase("settings", "SETTINGS") {}
};

} // namespace vsun::apps::settings
