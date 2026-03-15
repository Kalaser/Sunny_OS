#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <mutex>

#include "services/datacenter.h"

namespace vsun::services {

// UI-thread-only DataCenter publisher fed by any-thread producers.
//
// Threading model:
// - Worker threads MUST NOT call LVGL or DataCenter directly.
// - Worker threads enqueue (Topic + payload) here.
// - UI thread calls drain() once per tick (e.g. in AppController::update()) which
//   publishes to DataCenter in a single-threaded way.
//
// Notes:
// - Payload is copied into a fixed-size buffer. Keep payloads small PODs.
// - For RTOS targets, you can replace the internal implementation with an OS queue.
class UiPublishQueue {
public:
    static UiPublishQueue& instance();

    // Enqueue a raw payload. Returns false if payload too large.
    bool enqueue_raw(Topic topic, const void* data, std::size_t len);

    template <typename T>
    bool enqueue(Topic topic, const T& payload)
    {
        return enqueue_raw(topic, &payload, sizeof(T));
    }

    // Drain all pending events and publish them to DataCenter.
    // Must be called from the UI thread.
    void drain();

private:
    UiPublishQueue() = default;

    static constexpr std::size_t kMaxPayloadBytes = 64;

    struct Item {
        Topic topic{};
        std::uint16_t len = 0;
        std::array<std::uint8_t, kMaxPayloadBytes> bytes{};
    };

    std::mutex mu_{};
    std::deque<Item> q_{};
};

} // namespace vsun::services

