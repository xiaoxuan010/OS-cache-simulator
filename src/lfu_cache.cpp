#include "lfu_cache.h"

namespace cache_sim
{

    CacheLine *LFUCache::selectVictim(size_t set_index)
    {
        CacheSet &set = sets_[set_index];
        CacheLine *victim = nullptr;
        uint64_t min_count = UINT64_MAX;

        // 首先查找无效的缓存行
        for (auto &line : set.lines)
        {
            if (!line.valid)
            {
                return &line;
            }
        }

        stats_.conflicts++;
        // 选择最不经常使用的缓存行
        for (auto &line : set.lines)
        {
            if (line.access_count < min_count)
            {
                min_count = line.access_count;
                victim = &line;
            }
            else if (line.access_count == min_count && victim != nullptr && line.last_access_time < victim->last_access_time)
            {
                // 如果访问次数相同，选择最早访问的（LRU 作为辅助策略）
                victim = &line;
            }
        }

        return victim;
    }

    void LFUCache::updateAccessInfo(CacheLine *line)
    {
        if (line != nullptr)
        {
            line->access_count++;
            line->last_access_time = static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
        }
    }

    void LFUCache::resetLine(CacheLine *line)
    {
        if (line)
        {
            line->access_count = 0;
        }
    }

} // namespace cache_sim
