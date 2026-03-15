#include "services/ui_publish_queue.h"

#include <cstring>

namespace vsun::services {

UiPublishQueue& UiPublishQueue::instance()
{
    static UiPublishQueue q;
    return q;
}

bool UiPublishQueue::enqueue_raw(Topic topic, const void* data, std::size_t len)
{
    if(!data || len == 0) return false;
    if(len > kMaxPayloadBytes) return false;

    Item it{};
    it.topic = topic;
    it.len = (std::uint16_t)len;
    std::memcpy(it.bytes.data(), data, len);

    {
        std::lock_guard<std::mutex> lk(mu_);
        q_.push_back(it);
    }
    return true;
}

void UiPublishQueue::drain()
{
    // Swap to minimize lock hold time.
    std::deque<Item> local;
    {
        std::lock_guard<std::mutex> lk(mu_);
        local.swap(q_);
    }

    auto& dc = DataCenter::instance();
    for(const auto& it : local) {
        if(it.len == 0) continue;
        dc.publish(it.topic, it.bytes.data(), it.len);
    }
}

} // namespace vsun::services

