#include "services/datacenter.h"

#include <algorithm>

namespace vsun::services {

DataCenter& DataCenter::instance()
{
    static DataCenter dc;
    return dc;
}

DataCenter::Subscription DataCenter::subscribe(Topic topic, Callback cb, void* user)
{
    if(!cb) return {};
    const uint32_t id = next_id_++;
    subs_.push_back(SubRec{ id, topic, cb, user });
    return Subscription{ id };
}

void DataCenter::unsubscribe(Subscription sub)
{
    if(sub.id == 0) return;
    subs_.erase(
        std::remove_if(
            subs_.begin(),
            subs_.end(),
            [&](const SubRec& r) { return r.id == sub.id; }),
        subs_.end());
}

void DataCenter::publish(Topic topic, const void* data, size_t len)
{
    if(!data || len == 0) return;
    // 允许回调中 subscribe/unsubscribe，所以用索引遍历避免迭代器失效的常见坑。
    for(size_t i = 0; i < subs_.size(); i++) {
        const auto rec = subs_[i];
        if(rec.topic != topic || !rec.cb) continue;
        rec.cb(topic, data, len, rec.user);
    }
}

} // namespace vsun::services

