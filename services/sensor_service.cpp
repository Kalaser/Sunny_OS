#include "services/sensor_service.h"

#include "services/datacenter.h"

namespace vsun::services {

void SensorService::init()
{
    last_pub_ms_ = 0;
    t_ = 0.0f;
}

void SensorService::update(std::uint32_t now_ms)
{
    // Simulator mock: publish at ~20 Hz.
    if(last_pub_ms_ != 0 && (now_ms - last_pub_ms_) < 50) return;
    last_pub_ms_ = now_ms;

    // Very cheap fake waveform (no <cmath>) just to exercise the pipeline.
    t_ += 0.05f;
    if(t_ > 6.28f) t_ = 0.0f;

    AccelPayload p{};
    p.x = (t_ < 3.14f) ? (t_ / 3.14f) : (-(t_ - 3.14f) / 3.14f);
    p.y = -p.x;
    p.z = 1.0f;

    DataCenter::instance().publish(Topic::Accel, p);
}

} // namespace vsun::services

