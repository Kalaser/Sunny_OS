#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "services/datacenter_topics.h"

namespace vsun::services {

// A lightweight publish/subscribe bus (simulator-friendly implementation).
// Design goals:
// - UI/View does not depend on HAL or platform APIs; it only subscribes to topics.
// - Model can subscribe and cache state; slice is responsible for bind/unbind.
// - On RTOS, this can be replaced with a no-dynamic-allocation implementation.
class DataCenter {
public:
    using Callback = void (*)(Topic topic, const void* data, size_t len, void* user);

    struct Subscription {
        uint32_t id = 0;
    };

    static DataCenter& instance();

    Subscription subscribe(Topic topic, Callback cb, void* user);
    void unsubscribe(Subscription sub);

    void publish(Topic topic, const void* data, size_t len);

    template <typename T>
    void publish(Topic topic, const T& payload)
    {
        publish(topic, &payload, sizeof(T));
    }

private:
    struct SubRec {
        uint32_t id = 0;
        Topic topic{};
        Callback cb = nullptr;
        void* user = nullptr;
    };

    uint32_t next_id_ = 1;
    std::vector<SubRec> subs_;
};

} // namespace vsun::services

