#ifndef CACHE_SIMULATOR_H
#define CACHE_SIMULATOR_H

#include "cache.h"
#include "bus.h"
#include <bits/stdc++.h>

namespace cache_sim
{
    // 访问模式
    enum class AccessPattern
    {
        Random,     // 随机访问
        Sequential, // 顺序访问
        Localized   // 局部性访问
    };

    // 缓存替换策略
    enum class ReplacementPolicy
    {
        LRU,
        LFU
    };

    // 模拟器配置
    struct SimulatorConfig
    {
        CacheConfig cache_config;
        size_t num_accesses;          // 访问次数
        size_t address_range;         // 地址范围
        AccessPattern access_pattern; // 访问模式
        ReplacementPolicy replacement_policy; // 替换策略
        int num_cores;                        // 核心数量
        size_t working_set_period;            // 工作集切换周期（访问次数）

        // 获取当前替换策略的名称
        static std::string getPolicyName(ReplacementPolicy policy);

        SimulatorConfig(size_t accesses = 10000, size_t range = 1048576, AccessPattern pattern = AccessPattern::Random, ReplacementPolicy policy = ReplacementPolicy::LRU, int cores = 1, size_t ws_period = 10000)
            : num_accesses(accesses), address_range(range), access_pattern(pattern), replacement_policy(policy), num_cores(cores), working_set_period(ws_period)
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
        std::unique_ptr<Bus> bus_;
        std::vector<std::unique_ptr<Cache>> caches_;

        // 创建缓存实例
        void createCaches();

        // 生成访问地址
        uint64_t generateAddress(size_t index) const;

        // 执行单次访问
        void performAccess(size_t core_id, uint64_t address, bool is_write);
    };

} // namespace cache_sim

#endif // CACHE_SIMULATOR_H
