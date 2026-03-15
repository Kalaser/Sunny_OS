#include "services/system_status_service.h"

#include "port/time.h"
#include "services/datacenter.h"
#include "sunny_hal/hal.h"

namespace vsun::services {

void SystemStatusService::init()
{
    last_pub_ms_ = 0;
    vsun::hal::init();
}

void SystemStatusService::update(std::uint32_t now_ms)
{
    if(last_pub_ms_ != 0 && (now_ms - last_pub_ms_) < 1000) return;
    last_pub_ms_ = now_ms;

    // Let HAL update any internal state first.
    vsun::hal::update(now_ms);

    TimeHHMMPayload time_payload{};
    {
        char hhmm[6]{};
        if(vsun::port::get_local_time_hhmm(hhmm)) {
            for(int i = 0; i < 6; i++) time_payload.text[i] = hhmm[i];
        } else {
            time_payload.text[0] = '-';
            time_payload.text[1] = '-';
            time_payload.text[2] = ':';
            time_payload.text[3] = '-';
            time_payload.text[4] = '-';
            time_payload.text[5] = '\0';
        }
    }

    BatteryPercentPayload batt_payload{};
    batt_payload.percent = vsun::hal::battery_percent();

    auto& dc = DataCenter::instance();
    dc.publish(Topic::TimeHHMM, time_payload);
    dc.publish(Topic::BatteryPercent, batt_payload);
}

} // namespace vsun::services
