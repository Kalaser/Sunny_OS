#pragma once

#include <cstdint>

namespace vsun::services {

// Periodically publishes system status to DataCenter (time, battery, etc).
// Keep this non-UI and platform-agnostic; the actual device reads happen via
// vsun::port and vsun::hal.
class SystemStatusService {
public:
    void init();
    void update(std::uint32_t now_ms);

private:
    std::uint32_t last_pub_ms_ = 0;
};

} // namespace vsun::services

