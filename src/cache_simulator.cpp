#include "cache_simulator.h"
#include "lru_cache.h"
#include "bits/stdc++.h"

namespace cache_sim
{

    CacheSimulator::CacheSimulator(const SimulatorConfig &config)
        : config_(config)
    {
        createCache();
    }

    void CacheSimulator::createCache()
    {
        cache_ = std::make_unique<LRUCache>(config_.cache_config);
    }

    void CacheSimulator::run()
    {
        std::cout << "开始缓存模拟..." << std::endl;
        std::cout << "访问次数: " << config_.num_accesses << std::endl;

        for (size_t i = 0; i < config_.num_accesses; ++i)
        {
            uint64_t address = generateAddress();
            bool is_write = (i % 4 == 0); // 模拟 25% 的写操作
            performAccess(address, is_write);
        }

        std::cout << "模拟完成!" << std::endl;
    }

    void CacheSimulator::printResults() const
    {
        const CacheStats &stats = cache_->getStats();
        const CacheConfig &config = cache_->getConfig();

        std::cout << std::endl;
        std::cout << "========== 缓存模拟结果 ==========" << std::endl;
        std::cout << std::endl;

        std::cout << "--- 缓存配置 ---" << std::endl;
        std::cout << "缓存大小: " << config.cache_size << " 字节 ("
                  << config.cache_size / 1024 << " KB)" << std::endl;
        std::cout << "块大小: " << config.block_size << " 字节" << std::endl;
        std::cout << "关联度: " << config.associativity << " 路组相联" << std::endl;
        std::cout << std::endl;

        std::cout << "--- 访问统计 ---" << std::endl;
        std::cout << "读操作次数: " << stats.reads << std::endl;
        std::cout << "写操作次数: " << stats.writes << std::endl;
        std::cout << "缓存命中: " << stats.hits << std::endl;
        std::cout << "缓存缺失: " << stats.misses << std::endl;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "命中率: " << stats.hitRate() * 100 << "%" << std::endl;
        std::cout << std::endl;
        std::cout << "==================================" << std::endl;
    }

    uint64_t CacheSimulator::generateAddress() const
    {
        // 随机种子
        static std::mt19937_64 rng(20232005046);

        // 随机访问
        std::uniform_int_distribution<uint64_t> dist(0, config_.address_range - 1);
        return dist(rng);
    }

    void CacheSimulator::performAccess(uint64_t address, bool is_write)
    {
        if (is_write)
        {
            cache_->write(address, 0);
        }
        else
        {
            cache_->read(address);
        }
    }

} // namespace cache_sim
