#include "cache.h"
#include <bits/stdc++.h>

namespace cache_sim
{
    // 缓存构造函数
    Cache::Cache(const CacheConfig &config, int id, Bus *bus)
        : config_(config), id_(id), bus_(bus)
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

    // 读取数据 (实现 MESI 协议)
    bool Cache::read(uint64_t address)
    {
        stats_.reads++;

        CacheLine *line = findLine(address);
        if (line != nullptr)
        {
            // 缓存命中
            stats_.hits++;
            updateAccessInfo(line);
            // 状态保持不变 (M, E, S 都可以读)
            return true;
        }

        // 缓存缺失
        stats_.misses++;

        // 选择要替换的缓存行
        size_t set_index = getSetIndex(address);
        CacheLine *victim = selectVictim(set_index);

        // 重置被驱逐的行
        resetLine(victim);

        // 广播读请求 (BusRd)
        bool is_shared = false;
        if (bus_)
        {
            is_shared = bus_->broadcast(id_, address, BusEvent::BusRd);
        }

        // 模拟加载数据到缓存行
        victim->valid = true;
        victim->tag = getTag(address);
        victim->dirty = false;

        // 根据总线响应设置状态
        if (is_shared)
        {
            victim->state = MESIState::Shared;
        }
        else
        {
            victim->state = MESIState::Exclusive;
        }

        updateAccessInfo(victim);

        return false;
    }

    // 写入数据 (实现 MESI 协议)
    bool Cache::write(uint64_t address, uint8_t /*value*/)
    {
        stats_.writes++;

        CacheLine *line = findLine(address);
        if (line != nullptr)
        {
            // 缓存命中
            stats_.hits++;
            updateAccessInfo(line);

            // 如果是 Shared 状态，需要升级为 Modified
            if (line->state == MESIState::Shared)
            {
                // 广播 BusRdX 使其他缓存失效
                if (bus_)
                {
                    bus_->broadcast(id_, address, BusEvent::BusRdX);
                }
                line->state = MESIState::Modified;
            }
            else if (line->state == MESIState::Exclusive)
            {
                // E -> M
                line->state = MESIState::Modified;
            }
            // 如果已经是 Modified，状态不变

            line->dirty = true;
            return true;
        }

        // 缓存缺失
        stats_.misses++;

        // 选择要替换的缓存行
        size_t set_index = getSetIndex(address);
        CacheLine *victim = selectVictim(set_index);

        // 重置被驱逐的行
        resetLine(victim);

        // 广播写请求 (BusRdX)
        if (bus_)
        {
            bus_->broadcast(id_, address, BusEvent::BusRdX);
        }

        // 写入数据到缓存行
        victim->valid = true;
        victim->tag = getTag(address);
        victim->dirty = true;
        victim->state = MESIState::Modified;
        updateAccessInfo(victim);

        return false;
    }

    // 嗅探总线请求
    bool Cache::snoop(uint64_t address, BusEvent event)
    {
        CacheLine *line = findLine(address);
        if (line == nullptr)
        {
            return false;
        }

        // 命中，根据 MESI 协议更新状态
        switch (event)
        {
        case BusEvent::BusRd:
            // 远程读请求
            if (line->state == MESIState::Modified)
            {
                // M -> S，需要写回内存（Flush）
                line->dirty = false;
                line->state = MESIState::Shared;
            }
            else if (line->state == MESIState::Exclusive)
            {
                // E -> S
                line->state = MESIState::Shared;
            }
            // S -> S, I -> I (不变)
            break;

        case BusEvent::BusRdX:
            // 远程写请求（独占读）
            // 本地副本失效
            line->valid = false;
            line->state = MESIState::Invalid;
            break;
        }

        return true; // 返回 true 表示我们有这个数据（用于告知请求者是否 Shared）
    }

} // namespace cache_sim
