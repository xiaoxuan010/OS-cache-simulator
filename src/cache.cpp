#include "cache.h"
#include <bits/stdc++.h>

namespace cache_sim
{
    // 缓存构造函数
    Cache::Cache(const CacheConfig &config) : config_(config)
    {
        // 每组的大小
        size_t set_size = config_.block_size * config_.associativity;

        // 组数
        size_t num_sets = config_.cache_size / set_size;

        // 初始化缓存组
        sets_.reserve(num_sets);
        for (size_t i = 0; i < num_sets; i++)
        {
            sets_.emplace_back(config_.associativity, config_.block_size);
        }
    }

    // 从地址计算组索引（Set Index）
    size_t Cache::getSetIndex(uint64_t address) const
    {
        size_t num_sets = sets_.size();
        size_t block_bits = static_cast<size_t>(std::log2(config_.block_size));
        return (address >> block_bits) % num_sets;
    }

    // 从地址计算标签（Tag）
    uint64_t Cache::getTag(uint64_t address) const
    {
        size_t block_bits = static_cast<size_t>(std::log2(config_.block_size));
        size_t set_bits = static_cast<size_t>(std::log2(sets_.size()));
        return address >> (block_bits + set_bits);
    }

    // 从地址计算块内偏移（Block Offset）
    size_t Cache::getBlockOffset(uint64_t address) const
    {
        return address & (config_.block_size - 1);
    }

    // 查找缓存行
    CacheLine *Cache::findLine(uint64_t address)
    {
        size_t set_index = getSetIndex(address);
        uint64_t tag = getTag(address);

        // 在对应的组中，遍历查找指定 Tag 的缓存行
        for (auto &line : sets_[set_index].lines)
        {
            if (line.valid && line.tag == tag)
            {
                return &line;
            }
        }
        return nullptr;
    }

    // 重置统计信息
    void Cache::resetStats()
    {
        stats_ = CacheStats();
    }
}
