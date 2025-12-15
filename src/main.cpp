#include <bits/stdc++.h>
#include "cache_simulator.h"

using namespace cache_sim;

/**
 * @brief 打印使用帮助
 * @param program_name 程序名称
 */
void printUsage(const char *program_name)
{
    std::cout << "用法: " << program_name << " [选项]" << std::endl;
    std::cout << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  -h, --help              显示帮助信息" << std::endl;
    std::cout << "  -s, --size <字节>       缓存大小（默认: 32768，即 32KB）" << std::endl;
    std::cout << "  -b, --block <字节>      块大小（默认: 64）" << std::endl;
    std::cout << "  -a, --assoc <数值>      关联度（默认: 4，即 4 路组相联）" << std::endl;
    std::cout << "  -p, --policy <策略>     替换策略: lru 或 lfu（默认: lru）" << std::endl;
    std::cout << "  -t, --pattern <模式>    访问模式: random, sequential, locality（默认: random）" << std::endl;
    std::cout << "  -n, --accesses <次数>   访问次数（默认: 10000）" << std::endl;
    std::cout << "  -r, --range <字节>      地址范围（默认: 1048576，即 1MB）" << std::endl;
    std::cout << "  -c, --cores <数量>      CPU 核心数（默认: 1）" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  " << program_name << " -s 65536 -b 64 -a 4 -p lru -t random -n 10000" << std::endl;
    std::cout << "  " << program_name << " --size 32768 --policy lfu --pattern locality" << std::endl;
}

/**
 * @brief 解析命令行参数
 * @param argc 参数数量
 * @param argv 参数数组
 * @param config 配置结构体的引用
 * @return 是否解析成功
 */
bool parseArguments(int argc, char *argv[], SimulatorConfig &config)
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help")
        {
            printUsage(argv[0]);
            return false;
        }
        else if (arg == "-s" || arg == "--size")
        {
            if (++i >= argc)
            {
                std::cerr << "错误: 缺少缓存大小参数" << std::endl;
                return false;
            }
            config.cache_config.cache_size = std::stoul(argv[i]);
        }
        else if (arg == "-b" || arg == "--block")
        {
            if (++i >= argc)
            {
                std::cerr << "错误: 缺少块大小参数" << std::endl;
                return false;
            }
            config.cache_config.block_size = std::stoul(argv[i]);
        }
        else if (arg == "-a" || arg == "--assoc")
        {
            if (++i >= argc)
            {
                std::cerr << "错误: 缺少关联度参数" << std::endl;
                return false;
            }
            config.cache_config.associativity = std::stoul(argv[i]);
        }
        else if (arg == "-p" || arg == "--policy")
        {
            if (++i >= argc)
            {
                std::cerr << "错误: 缺少替换策略参数" << std::endl;
                return false;
            }
            std::string policy = argv[i];
            if (policy == "lru" || policy == "LRU")
            {
                config.replacement_policy = ReplacementPolicy::LRU;
            }
            else if (policy == "lfu" || policy == "LFU")
            {
                config.replacement_policy = ReplacementPolicy::LFU;
            }
            else
            {
                std::cerr << "错误: 未知的替换策略 '" << policy << "'" << std::endl;
                return false;
            }
        }
        else if (arg == "-t" || arg == "--pattern")
        {
            if (++i >= argc)
            {
                std::cerr << "错误: 缺少访问模式参数" << std::endl;
                return false;
            }
            std::string pattern = argv[i];
            if (pattern == "random")
            {
                config.access_pattern = AccessPattern::Random;
            }
            else if (pattern == "sequential")
            {
                config.access_pattern = AccessPattern::Sequential;
            }
            else if (pattern == "locality")
            {
                config.access_pattern = AccessPattern::Localized;
            }
            else
            {
                std::cerr << "错误: 未知的访问模式 '" << pattern << "'" << std::endl;
                return false;
            }
        }
        else if (arg == "-n" || arg == "--accesses")
        {
            if (++i >= argc)
            {
                std::cerr << "错误: 缺少访问次数参数" << std::endl;
                return false;
            }
            config.num_accesses = std::stoul(argv[i]);
        }
        else if (arg == "-r" || arg == "--range")
        {
            if (++i >= argc)
            {
                std::cerr << "错误: 缺少地址范围参数" << std::endl;
                return false;
            }
            config.address_range = std::stoul(argv[i]);
        }
        else if (arg == "-c" || arg == "--cores")
        {
            if (++i >= argc)
            {
                std::cerr << "错误: 缺少核心数参数" << std::endl;
                return false;
            }
            config.num_cores = std::stoul(argv[i]);
        }
        else
        {
            std::cerr << "错误: 未知的选项 '" << arg << "'" << std::endl;
            printUsage(argv[0]);
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[])
{
    std::cout << "OS-cache-simulator: 基于 LRU 的用户态缓存系统模拟器" << std::endl;

    SimulatorConfig config;

    if (!parseArguments(argc, argv, config))
    {
        return (argc > 1 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) ? 0 : 1;
    }

    CacheSimulator simulator(config);
    simulator.run();
    simulator.printResults();

    return 0;
}
