#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include "cache.h"
#include <bits/stdc++.h>

namespace cache_sim
{

    // LRU 缓存实现
    class LRUCache : public Cache
    {
    public:
        LRUCache(const CacheConfig &config, int id = 0, Bus *bus = nullptr);
        ~LRUCache() override = default;

        CacheLine *selectVictim(size_t set_index) override;
        void updateAccessInfo(size_t set_index, CacheLine *line) override;
        void resetLine(size_t set_index, CacheLine *line) override;

    private:
        struct LRUSet
        {
            std::list<CacheLine *> lru_list;
            std::unordered_map<CacheLine *, std::list<CacheLine *>::iterator> line_to_node;
        };

        std::vector<LRUSet> lru_sets_;
    };

} // namespace cache_sim

#endif // LRU_CACHE_H
