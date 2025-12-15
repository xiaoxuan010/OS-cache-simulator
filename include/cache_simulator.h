#ifndef CACHE_SIMULATOR_H
#define CACHE_SIMULATOR_H

#include "cache.h"
#include <bits/stdc++.h>

namespace cache_sim
{
    enum class AccessPattern
    {
        Random,     // 随机访问
        Sequential, // 顺序访问
        Localized   // 局部性访问
    };

    // 模拟器配置
    struct SimulatorConfig
    {
        CacheConfig cache_config;
        size_t num_accesses;          // 访问次数
        size_t address_range;         // 地址范围
        AccessPattern access_pattern; // 访问模式

        SimulatorConfig()
            : num_accesses(10000), address_range(1048576), access_pattern(AccessPattern::Random) // 1MB
        {
        }
    };

    // 缓存模拟器
    class CacheSimulator
    {
    public:
        explicit CacheSimulator(const SimulatorConfig &config);
        ~CacheSimulator() = default;

        // 运行模拟
        void run();

        // 打印结果
        void printResults() const;

        // 获取当前访问模式的名称
        static std::string getPatterName(AccessPattern pattern);

    private:
        SimulatorConfig config_;
        std::unique_ptr<Cache> cache_;

        // 创建缓存实例
        void createCache();

        // 生成访问地址
        uint64_t generateAddress(size_t index) const;

        // 执行单次访问
        void performAccess(uint64_t address, bool is_write);
    };

} // namespace cache_sim

#endif // CACHE_SIMULATOR_H
