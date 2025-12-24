#ifndef LFU_CACHE_H
#define LFU_CACHE_H

#include "cache.h"
#include <bits/stdc++.h>

namespace cache_sim
{

    // LFU 缓存实现
    class LFUCache : public Cache
    {
    public:
        LFUCache(const CacheConfig &config, int id = 0, Bus *bus = nullptr);
        ~LFUCache() override = default;

        CacheLine *selectVictim(size_t set_index) override;
        void updateAccessInfo(size_t set_index, CacheLine *line) override;
        void resetLine(size_t set_index, CacheLine *line) override;

    private:
        struct LFUSet
        {
            // 频率 -> 缓存行列表
            std::unordered_map<uint64_t, std::list<CacheLine *>> freq_list;
            // 缓存行 -> 列表迭代器
            std::unordered_map<CacheLine *, std::list<CacheLine *>::iterator> line_to_node;
            uint64_t min_freq = 0;
        };

        std::vector<LFUSet> lfu_sets_;
    };

} // namespace cache_sim

#endif // LFU_CACHE_H
