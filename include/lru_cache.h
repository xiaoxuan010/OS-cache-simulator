#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include "cache.h"

namespace cache_sim
{

    // LRU 缓存实现
    class LRUCache : public Cache
    {
    public:
        using Cache::Cache;
        ~LRUCache() override = default;

        CacheLine *selectVictim(size_t set_index) override;
        void updateAccessInfo(CacheLine *line) override;
        void resetLine(CacheLine *line) override;
    };

} // namespace cache_sim

#endif // LRU_CACHE_H
