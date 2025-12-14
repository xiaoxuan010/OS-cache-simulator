#include "lru_cache.h"

namespace cache_sim
{

    LRUCache::LRUCache(const CacheConfig &config)
        : Cache(config)
    {
    }

    bool LRUCache::read(uint64_t address)
    {
        stats_.reads++;

        CacheLine *line = findLine(address);
        if (line != nullptr)
        {
            // 缓存命中
            stats_.hits++;
            updateAccessInfo(line);
            return true;
        }

        // 缓存缺失
        stats_.misses++;

        // 选择要替换的缓存行
        size_t set_index = getSetIndex(address);
        CacheLine *victim = selectVictim(set_index);

        // 模拟加载数据到缓存行
        victim->valid = true;
        victim->tag = getTag(address);
        victim->dirty = false;
        victim->state = MESIState::Exclusive;
        updateAccessInfo(victim);

        return false;
    }

    bool LRUCache::write(uint64_t address, uint8_t /*value*/)
    {
        stats_.writes++;

        CacheLine *line = findLine(address);
        if (line != nullptr)
        {
            // 缓存命中
            stats_.hits++;
            line->dirty = true;
            line->state = MESIState::Modified;
            updateAccessInfo(line);
            return true;
        }

        // 缓存缺失
        stats_.misses++;

        // 选择要替换的缓存行
        size_t set_index = getSetIndex(address);
        CacheLine *victim = selectVictim(set_index);

        // 模拟写入数据到缓存行
        victim->valid = true;
        victim->tag = getTag(address);
        victim->dirty = true;
        victim->state = MESIState::Modified;
        updateAccessInfo(victim);

        return false;
    }

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

} // namespace cache_sim
