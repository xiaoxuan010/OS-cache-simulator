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
        std::cout << "访问模式: " << getPatterName(config_.access_pattern) << std::endl;
        std::cout << "访问次数: " << config_.num_accesses << std::endl;

        for (size_t i = 0; i < config_.num_accesses; ++i)
        {
            uint64_t address = generateAddress(i);
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

    uint64_t CacheSimulator::generateAddress(size_t index) const
    {
        // 随机种子
        static std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());

        switch (config_.access_pattern)
        {
        case AccessPattern::Random:
        {
            std::uniform_int_distribution<uint64_t> dist(0, config_.address_range - 1);
            return dist(rng);
        }
        case AccessPattern::Sequential:
        {
            return (index * 4) % config_.address_range;
        }    
        case AccessPattern::Localized:
        {
            // 模拟局部性：90% 的访问在小范围内，10% 随机访问
            std::uniform_real_distribution<double> prob_dist(0.0, 1.0);
            if (prob_dist(rng) < 0.9)
            {
                // 局部访问：在当前工作集附近
                size_t working_set_size = config_.cache_config.cache_size;
                size_t base = (index / 1000) * working_set_size;
                std::uniform_int_distribution<uint64_t> local_dist(0, working_set_size - 1);
                return (base + local_dist(rng)) % config_.address_range;
            }
            else
            {
                // 随机访问
                std::uniform_int_distribution<uint64_t> dist(0, config_.address_range - 1);
                return dist(rng);
            }
        }
        default:
            return 0;
        }
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

    std::string CacheSimulator::getPatterName(AccessPattern pattern)
    {
        switch (pattern)
        {
        case AccessPattern::Random:
            return "随机访问";
        case AccessPattern::Sequential:
            return "顺序访问";
        case AccessPattern::Localized:
            return "局部性访问";
        default:
            return "未知模式";
        }
    }

} // namespace cache_sim
