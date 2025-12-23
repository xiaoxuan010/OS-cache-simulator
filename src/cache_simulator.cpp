#include "cache_simulator.h"
#include "lru_cache.h"
#include "lfu_cache.h"
#include "bits/stdc++.h"

namespace cache_sim
{

    CacheSimulator::CacheSimulator(const SimulatorConfig &config)
        : config_(config)
    {
        createCaches();
    }

    void CacheSimulator::createCaches()
    {
        bus_ = std::make_unique<Bus>();
        caches_.reserve(config_.num_cores);

        for (int i = 0; i < config_.num_cores; ++i)
        {
            std::unique_ptr<Cache> cache;
            switch (config_.replacement_policy)
            {
            case ReplacementPolicy::LRU:
                cache = std::make_unique<LRUCache>(config_.cache_config, i, bus_.get());
                break;
            case ReplacementPolicy::LFU:
                cache = std::make_unique<LFUCache>(config_.cache_config, i, bus_.get());
                break;

            default:
                std::cerr << "[Warning] 未知的替换策略，使用默认的 LRU 策略。" << std::endl;
                cache = std::make_unique<LRUCache>(config_.cache_config, i, bus_.get());
                break;
            }

            bus_->attach(cache.get());
            caches_.push_back(std::move(cache));
        }
    }

    void CacheSimulator::run()
    {
        // 随机种子用于选择核心
        static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> core_dist(0, config_.num_cores - 1);

        for (size_t i = 0; i < config_.num_accesses; ++i)
        {
            uint64_t address = generateAddress(i);
            bool is_write = (i % 4 == 0);    // 模拟 25% 的写操作
            size_t core_id = core_dist(rng); // 随机选择一个核心发起请求
            performAccess(core_id, address, is_write);
        }
    }

    void CacheSimulator::printResults() const
    {
        if (config_.output_json)
        {
            std::ostringstream oss;
            oss << "{\n  \"cores\": [\n";
            for (int i = 0; i < config_.num_cores; ++i)
            {
                const CacheStats &stats = caches_[i]->getStats();
                double hit_rate = stats.hitRate() * 100.0;
                double conflict_rate = stats.conflictRate() * 100.0;
                oss << "    {\n"
                    << "      \"core_id\": " << i << ",\n"
                    << "      \"reads\": " << stats.reads << ",\n"
                    << "      \"writes\": " << stats.writes << ",\n"
                    << "      \"hits\": " << stats.hits << ",\n"
                    << "      \"misses\": " << stats.misses << ",\n"
                    << "      \"hit_rate\": " << std::fixed << std::setprecision(2) << hit_rate << ",\n"
                    << "      \"conflicts\": " << stats.conflicts << ",\n"
                    << "      \"conflict_rate\": " << std::fixed << std::setprecision(2) << conflict_rate << "\n"
                    << "    }" << (i + 1 == config_.num_cores ? "\n" : ",\n");
            }
            CacheStats avg_stats = getAverageStats();
            double avg_hit_rate = avg_stats.hitRate() * 100.0;
            double avg_conflict_rate = avg_stats.conflictRate() * 100.0;
            oss << "  ],\n"
                << "  \"average\": {\n"
                << "    \"reads\": " << avg_stats.reads << ",\n"
                << "    \"writes\": " << avg_stats.writes << ",\n"
                << "    \"hits\": " << avg_stats.hits << ",\n"
                << "    \"misses\": " << avg_stats.misses << ",\n"
                << "    \"hit_rate\": " << std::fixed << std::setprecision(2) << avg_hit_rate << ",\n"
                << "    \"conflicts\": " << avg_stats.conflicts << ",\n"
                << "    \"conflict_rate\": " << std::fixed << std::setprecision(2) << avg_conflict_rate << "\n"
                << "  }\n"
                << "}\n";
            std::cout << oss.str();
        }
        else
        {
            std::cout << "========== 缓存模拟结果 ==========" << std::endl;
            std::cout << "--- 模拟器配置 ---" << std::endl;
            std::cout << "核心数量: " << config_.num_cores << std::endl;
            std::cout << "替换策略: " << SimulatorConfig::getPolicyName(config_.replacement_policy) << std::endl;
            std::cout << "访问模式: " << getPatterName(config_.access_pattern) << std::endl;
            std::cout << "访问次数: " << config_.num_accesses << std::endl;
            std::cout << std::endl;

            const CacheConfig &config = caches_[0]->getConfig();
            std::cout << "--- 缓存配置 ---" << std::endl;
            std::cout << "缓存大小: " << config.cache_size << " 字节 ("
                      << config.cache_size / 1024 << " KB)" << std::endl;
            std::cout << "块大小: " << config.block_size << " 字节" << std::endl;
            std::cout << "关联度: " << config.associativity << " 路组相联" << std::endl;
            std::cout << std::endl;

            for (int i = 0; i < config_.num_cores; ++i)
            {
                const CacheStats &stats = caches_[i]->getStats();
                std::cout << "--- Core " << i << " 统计 ---" << std::endl;
                std::cout << "读操作次数: " << stats.reads << std::endl;
                std::cout << "写操作次数: " << stats.writes << std::endl;
                std::cout << "缓存命中: " << stats.hits << std::endl;
                std::cout << "缓存缺失: " << stats.misses << std::endl;
                std::cout << std::fixed << std::setprecision(2);
                std::cout << "命中率: " << stats.hitRate() * 100 << "%" << std::endl;
                std::cout << "冲突次数: " << stats.conflicts << std::endl;
                std::cout << "冲突率: " << stats.conflictRate() * 100 << "%" << std::endl;
                std::cout << std::endl;
            }

            std::cout << "--- 平均统计 ---" << std::endl;
            CacheStats avg_stats = getAverageStats();
            std::cout << "读操作次数: " << avg_stats.reads << std::endl;
            std::cout << "写操作次数: " << avg_stats.writes << std::endl;
            std::cout << "缓存命中: " << avg_stats.hits << std::endl;
            std::cout << "缓存缺失: " << avg_stats.misses << std::endl;
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "命中率: " << avg_stats.hitRate() * 100 << "%" << std::endl;
            std::cout << "冲突次数: " << avg_stats.conflicts << std::endl;
            std::cout << "冲突率: " << avg_stats.conflictRate() * 100 << "%" << std::endl;
            
            std::cout << "==================================" << std::endl;
        }
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
            return (index * caches_[0]->getConfig().block_size) % config_.address_range;
        }
        case AccessPattern::Localized:
        {
            // 模拟局部性：90% 的访问在小范围内，10% 随机访问
            std::uniform_real_distribution<double> prob_dist(0.0, 1.0);
            if (prob_dist(rng) < 0.9)
            {
                // 局部访问：在当前工作集附近
                size_t working_set_size = config_.working_set_size;
                size_t base = (index / config_.working_set_period) * working_set_size;
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

    void CacheSimulator::performAccess(size_t core_id, uint64_t address, bool is_write)
    {
        if (core_id >= caches_.size())
            return;

        if (is_write)
        {
            caches_[core_id]->write(address, 0);
        }
        else
        {
            caches_[core_id]->read(address);
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

    CacheStats CacheSimulator::getAverageStats() const
    {
        CacheStats avg_stats;
        for (const auto &cache : caches_)
        {
            const CacheStats &stats = cache->getStats();
            avg_stats.hits += stats.hits;
            avg_stats.misses += stats.misses;
            avg_stats.reads += stats.reads;
            avg_stats.writes += stats.writes;
            avg_stats.conflicts += stats.conflicts;
        }
        size_t num_caches = caches_.size();
        if (num_caches > 0)
        {
            avg_stats.hits /= num_caches;
            avg_stats.misses /= num_caches;
            avg_stats.reads /= num_caches;
            avg_stats.writes /= num_caches;
            avg_stats.conflicts /= num_caches;
        }
        return avg_stats;
    }

    std::string SimulatorConfig::getPolicyName(ReplacementPolicy policy)
    {
        switch (policy)
        {
        case ReplacementPolicy::LRU:
            return "LRU";
        case ReplacementPolicy::LFU:
            return "LFU";
        default:
            return "未知策略";
        }
    }

} // namespace cache_sim
