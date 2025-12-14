#include <bits/stdc++.h>
#include "cache_simulator.h"

int main()
{
    std::cout << "OS-cache-simulator: 基于 LRU 的用户态缓存系统模拟器" << std::endl;

    cache_sim::SimulatorConfig sim_config;
    sim_config.num_accesses = 50000;    // 访问次数
    sim_config.address_range = 1 << 20; // 1MB 地址范围

    cache_sim::CacheSimulator simulator(sim_config);
    simulator.run();
    simulator.printResults();

    return 0;
}
