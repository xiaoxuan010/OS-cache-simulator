#include "lru_cache.h"

namespace cache_sim
{

    CacheLine *LRUCache::selectVictim(size_t set_index)
    {
        CacheSet &set = sets_[set_index];
        CacheLine *victim = nullptr;
        uint64_t min_time = UINT64_MAX;

        // 首先查找无效的缓存行
        for (auto &line : set.lines)
        {
            if (!line.valid)
            {
                return &line;
            }
        }

        stats_.conflicts++;
        // 选择最近最少使用的缓存行
        for (auto &line : set.lines)
        {
            if (line.last_access_time < min_time)
            {
                min_time = line.last_access_time;
                victim = &line;
            }
        }

        return victim;
    }

    void LRUCache::updateAccessInfo(CacheLine *line)
    {
        if (line != nullptr)
        {
            line->last_access_time = std::chrono::steady_clock::now().time_since_epoch().count();
        }
    }

    void LRUCache::resetLine(CacheLine *)
    {
        // 什么都不用做，因为 LRU 不需要额外重置信息
    }

} // namespace cache_sim
