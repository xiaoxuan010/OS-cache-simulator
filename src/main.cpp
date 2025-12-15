#include <bits/stdc++.h>
#include "cache_simulator.h"

int main()
{
    std::cout << "OS-cache-simulator: 基于 LRU 的用户态缓存系统模拟器" << std::endl;

    std::cout << "请输入访问次数：";
    size_t num_accesses;
    std::cin >> num_accesses;

    std::cout << "请输入地址范围（字节）：";
    size_t address_range;
    std::cin >> address_range;

    std::cout << "请选择访问模式（0=随机访问, 1=顺序访问, 2=局部性访问）：";
    int pattern_choice;
    std::cin >> pattern_choice;
    cache_sim::AccessPattern access_pattern;
    switch (pattern_choice)
    {
    case 0:
        access_pattern = cache_sim::AccessPattern::Random;
        break;
    case 1:
        access_pattern = cache_sim::AccessPattern::Sequential;
        break;
    case 2:
        access_pattern = cache_sim::AccessPattern::Localized;
        break;
    default:
        std::cout << "无效选择，使用默认的随机访问模式。" << std::endl;
        access_pattern = cache_sim::AccessPattern::Random;
        break;
    }

    std::cout << "请选择替换策略（0=LRU, 1=LFU）：";
    int policy_choice;
    std::cin >> policy_choice;
    cache_sim::ReplacementPolicy replacement_policy;
    switch (policy_choice)
    {
    case 0:
        replacement_policy = cache_sim::ReplacementPolicy::LRU;
        break;
    case 1:
        replacement_policy = cache_sim::ReplacementPolicy::LFU;
        break;
    default:
        std::cout << "无效选择，使用默认的 LRU 策略。" << std::endl;
        replacement_policy = cache_sim::ReplacementPolicy::LRU;
        break;
    }

    cache_sim::SimulatorConfig sim_config(num_accesses, address_range, access_pattern, replacement_policy);
    cache_sim::CacheSimulator simulator(sim_config);
    simulator.run();
    simulator.printResults();

    return 0;
}
