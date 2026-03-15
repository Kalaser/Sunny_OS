#pragma once

// ResourcePool: centralized resource initialization (placeholder).
// Goals:
// - Initialize fonts/images/theme resources in one place, not scattered across views.
// - Later can be backed by filesystem loading or packed resources.
namespace vsun::services {

class ResourcePool {
public:
    static void init();
    static void uninit();
};

} // namespace vsun::services

