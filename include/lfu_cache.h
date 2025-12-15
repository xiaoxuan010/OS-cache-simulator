#ifndef LFU_CACHE_H
#define LFU_CACHE_H

#include "cache.h"

namespace cache_sim
{

    // LFU 缓存实现
    class LFUCache : public Cache
    {
    public:
        explicit LFUCache(const CacheConfig &config);
        ~LFUCache() override = default;

        bool read(uint64_t address) override;
        bool write(uint64_t address, uint8_t value) override;
        
        CacheLine *selectVictim(size_t set_index) override;
        void updateAccessInfo(CacheLine *line) override;
    };

} // namespace cache_sim

#endif // LFU_CACHE_H
