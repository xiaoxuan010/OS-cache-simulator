#ifndef LFU_CACHE_H
#define LFU_CACHE_H

#include "cache.h"

namespace cache_sim
{

    // LFU 缓存实现
    class LFUCache : public Cache
    {
    public:
        using Cache::Cache;
        ~LFUCache() override = default;

        CacheLine *selectVictim(size_t set_index) override;
        void updateAccessInfo(size_t set_index, CacheLine *line) override;
        void resetLine(size_t set_index, CacheLine *line) override;
    };

} // namespace cache_sim

#endif // LFU_CACHE_H
