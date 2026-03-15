#pragma once

#include <cstdint>

namespace vsun::services {

// Sensor service skeleton (IMU, etc).
// For real targets, read sensors in HAL/drivers and publish via DataCenter.
class SensorService {
public:
    void init();
    void update(std::uint32_t now_ms);

private:
    std::uint32_t last_pub_ms_ = 0;
    float t_ = 0.0f;
};

} // namespace vsun::services

