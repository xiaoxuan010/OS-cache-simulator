#ifndef CACHE_H
#define CACHE_H

#include "cache_line.h"
#include <bits/stdc++.h>

namespace cache_sim
{
    // 缓存替换策略
    enum class ReplacementPolicy
    {
        LRU,
        LFU
    };

    // 缓存配置
    struct CacheConfig
    {
        size_t cache_size;        // 缓存总大小（字节）
        size_t block_size;        // 块大小（字节）
        size_t associativity;     // 关联度（1=直接映射, N=N路组相联）
        ReplacementPolicy policy; // 替换策略

        CacheConfig()
            : cache_size(32768) // 默认 32KB
              ,
              block_size(64) // 默认 64 字节
              ,
              associativity(4) // 默认 4 路组相联
              ,
              policy(ReplacementPolicy::LRU)
        {
        }

        CacheConfig(size_t c_size, size_t b_size, size_t assoc, ReplacementPolicy pol = ReplacementPolicy::LRU)
            : cache_size(c_size), block_size(b_size), associativity(assoc), policy(pol)
        {
        }
    };

    // 缓存统计信息
    struct CacheStats
    {
        uint64_t hits;   // 命中次数
        uint64_t misses; // 缺失次数
        uint64_t reads;  // 读操作次数
        uint64_t writes; // 写操作次数

        CacheStats() : hits(0), misses(0), reads(0), writes(0) {}

        // 计算命中率
        double hitRate() const
        {
            uint64_t total = hits + misses;
            return total > 0 ? static_cast<double>(hits) / total : 0.0;
        }
    };

    // 缓存基类
    class Cache
    {
    public:
        explicit Cache(const CacheConfig &config);
        virtual ~Cache() = default;

        // 获取配置
        const CacheConfig &getConfig() const { return config_; }

        // 计算组索引
        size_t getSetIndex(uint64_t address) const;

        // 计算标签
        uint64_t getTag(uint64_t address) const;

        // 计算块内偏移
        size_t getBlockOffset(uint64_t address) const;

        // 查找缓存行
        CacheLine *findLine(uint64_t address);

        // 重置统计信息
        void resetStats();

        // 读取数据
        virtual bool read(uint64_t address) = 0;

        // 写入数据
        virtual bool write(uint64_t address, uint8_t value) = 0;

        // 选择要替换的缓存行（由子类实现具体策略）
        virtual CacheLine *selectVictim(size_t set_index) = 0;

        // 更新访问信息（由子类实现）
        virtual void updateAccessInfo(CacheLine *line) = 0;

        // 获取统计信息
        const CacheStats &getStats() const { return stats_; }

    protected:
        // 缓存配置
        CacheConfig config_;

        // 缓存统计信息
        CacheStats stats_;

        // 缓存组
        std::vector<CacheSet> sets_;
    };

} // namespace cache_sim

#endif // CACHE_H
