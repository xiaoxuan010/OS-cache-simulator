#include "bus.h"
#include "cache.h"

namespace cache_sim
{

    void Bus::attach(Cache *cache)
    {
        caches_.push_back(cache);
    }

    bool Bus::broadcast(int sender_id, uint64_t address, BusEvent event)
    {
        bool is_shared = false;
        for (auto *cache : caches_)
        {
            // 跳过发送请求的缓存
            if (cache->getId() == sender_id)
            {
                continue;
            }

            // 调用其他缓存的嗅探函数
            if (cache->snoop(address, event))
            {
                is_shared = true;
            }
        }
        return is_shared;
    }

} // namespace cache_sim
