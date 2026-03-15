#include "services/connectivity_service.h"

#include "services/datacenter.h"
#include "services/ui_publish_queue.h"

namespace vsun::services {
namespace {

static WifiStatePayload make_wifi_payload(uint8_t phase)
{
    WifiStatePayload p{};
    if(phase == 0) {
        p.state = LinkState::Down;
        p.rssi_dbm = 0;
    } else if(phase == 1) {
        p.state = LinkState::Connecting;
        p.rssi_dbm = 0;
    } else {
        p.state = LinkState::Up;
        p.rssi_dbm = -55;
    }
    return p;
}

static BleStatePayload make_ble_payload(uint8_t phase)
{
    BleStatePayload p{};
    p.state = (phase >= 2) ? LinkState::Up : LinkState::Down;
    return p;
}

} // namespace

void ConnectivityService::init()
{
    last_step_ms_ = 0;
    phase_ = 0;
}

void ConnectivityService::update(std::uint32_t now_ms)
{
#if defined(SUNNY_CONNECTIVITY_USE_MOCK) && (SUNNY_CONNECTIVITY_USE_MOCK != 0)
    // Simulator mock: cycle states every 5 seconds.
    if(last_step_ms_ != 0 && (now_ms - last_step_ms_) < 5000) return;
    last_step_ms_ = now_ms;
    phase_ = (uint8_t)((phase_ + 1) % 3);

    auto& dc = DataCenter::instance();
    dc.publish(Topic::WifiState, make_wifi_payload(phase_));
    dc.publish(Topic::BleState, make_ble_payload(phase_));
#else
    (void)now_ms;
#endif
}

void ConnectivityService::report_wifi_state(LinkState state, int8_t rssi_dbm)
{
    WifiStatePayload p{};
    p.state = state;
    p.rssi_dbm = rssi_dbm;
    UiPublishQueue::instance().enqueue(Topic::WifiState, p);
}

void ConnectivityService::report_ble_state(LinkState state)
{
    BleStatePayload p{};
    p.state = state;
    UiPublishQueue::instance().enqueue(Topic::BleState, p);
}

} // namespace vsun::services
