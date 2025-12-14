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

    protected:
        // 缓存配置
        CacheConfig config_;

        // 缓存组
        std::vector<CacheSet> sets_;
    };

} // namespace cache_sim

#endif // CACHE_H
