#pragma once

#include <cstdint>

// DataCenter topics and payload definitions live here for easy sharing and porting.
namespace vsun::services {

enum class Topic : uint8_t {
    TimeHHMM = 1,
    BatteryPercent = 2,
    WifiState = 3,
    BleState = 4,
    Accel = 5,
};

enum class LinkState : uint8_t {
    Down = 0,
    Connecting = 1,
    Up = 2,
};

struct TimeHHMMPayload {
    // Fixed format "HH:MM" + '\0'
    char text[6]{};
};

struct BatteryPercentPayload {
    // 0..100
    uint8_t percent = 0;
};

struct WifiStatePayload {
    LinkState state{};
    // Typical range: [-100..0], or 0 when unknown.
    int8_t rssi_dbm = 0;
};

struct BleStatePayload {
    LinkState state{};
};

struct AccelPayload {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

} // namespace vsun::services

