#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include "cache.h"

namespace cache_sim
{

    // LRU 缓存实现
    class LRUCache : public Cache
    {
    public:
        explicit LRUCache(const CacheConfig &config);
        ~LRUCache() override = default;

        bool read(uint64_t address) override;
        bool write(uint64_t address, uint8_t value) override;
        CacheLine *selectVictim(size_t set_index) override;
        void updateAccessInfo(CacheLine *line) override;
    };

} // namespace cache_sim

#endif // LRU_CACHE_H
