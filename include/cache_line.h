#ifndef CACHE_LINE_H
#define CACHE_LINE_H

#include <bits/stdc++.h>

namespace cache_sim
{
    // MESI 状态协议
    enum class MESIState
    {
        Modified,  // 已修改，数据与主存不一致
        Exclusive, // 独占，数据与主存一致，仅此缓存有副本
        Shared,    // 共享，数据与主存一致，可能有其他缓存副本
        Invalid    // 无效，缓存行无效
    };

    // 缓存行
    struct CacheLine
    {
        bool valid;                // 有效位
        bool dirty;                // 脏位
        uint64_t tag;              // 标签
        MESIState state;           // MESI 状态
        std::vector<uint8_t> data; // 数据块
        uint64_t last_access_time; // 最后访问时间（用于 LRU）
        uint64_t access_count;     // 访问计数（用于 LFU）

        CacheLine(size_t block_size = 64)
            : valid(false), dirty(false), tag(0), state(MESIState::Invalid), data(block_size, 0), last_access_time(0), access_count(0) {}
    };

    // 缓存组
    struct CacheSet
    {
        std::vector<CacheLine> lines;

        CacheSet(size_t associativity, size_t block_size)
        {
            lines.reserve(associativity);
            for (size_t i = 0; i < associativity; ++i)
            {
                lines.emplace_back(block_size);
            }
        }
    };
}

#endif // CACHE_LINE_H
