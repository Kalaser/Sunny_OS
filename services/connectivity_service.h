#pragma once

#include <cstdint>

#include "services/datacenter_topics.h"

namespace vsun::services {

// Connectivity service skeleton (WiFi/BLE).
// For real targets, back this with platform drivers/SDK and publish state via DataCenter.
class ConnectivityService {
public:
    void init();
    void update(std::uint32_t now_ms);

    // Cross-platform integration points:
    // - Safe to call from any thread; data will be delivered on the UI thread via UiPublishQueue.
    void report_wifi_state(LinkState state, int8_t rssi_dbm);
    void report_ble_state(LinkState state);

private:
    std::uint32_t last_step_ms_ = 0;
    std::uint8_t phase_ = 0;
};

} // namespace vsun::services
