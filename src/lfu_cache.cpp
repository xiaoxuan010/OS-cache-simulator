#include "lfu_cache.h"

namespace cache_sim
{

    LFUCache::LFUCache(const CacheConfig &config)
        : Cache(config)
    {
    }

    bool LFUCache::read(uint64_t address)
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
        victim->access_count = 0; // 重置访问计数
        updateAccessInfo(victim);

        return false;
    }

    bool LFUCache::write(uint64_t address, uint8_t)
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
        victim->access_count = 0; // 重置访问计数
        updateAccessInfo(victim);

        return false;
    }

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
        }

        return victim;
    }

    void LFUCache::updateAccessInfo(CacheLine *line)
    {
        if (line != nullptr)
        {
            line->access_count++;
        }
    }

} // namespace cache_sim
