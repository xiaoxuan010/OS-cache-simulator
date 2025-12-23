#ifndef CACHE_H
#define CACHE_H

#include "cache_line.h"
#include "bus.h"
#include <bits/stdc++.h>

namespace cache_sim
{
    // 缓存配置
    struct CacheConfig
    {
        size_t cache_size;    // 缓存总大小（字节）
        size_t block_size;    // 块大小（字节）
        size_t associativity; // 关联度（1=直接映射, N=N路组相联）

        CacheConfig()
            : cache_size(32768) // 默认 32KB
              ,
              block_size(64) // 默认 64 字节
              ,
              associativity(4) // 默认 4 路组相联
        {
        }

        CacheConfig(size_t c_size, size_t b_size, size_t assoc)
            : cache_size(c_size), block_size(b_size), associativity(assoc)
        {
        }
    };

    // 缓存统计信息
    struct CacheStats
    {
        uint64_t hits;      // 命中次数
        uint64_t misses;    // 缺失次数
        uint64_t reads;     // 读操作次数
        uint64_t writes;    // 写操作次数
        uint64_t conflicts; // 冲突次数

        CacheStats() : hits(0), misses(0), reads(0), writes(0), conflicts(0) {}

        // 计算命中率
        double hitRate() const
        {
            uint64_t total = hits + misses;
            return total > 0 ? static_cast<double>(hits) / total : 0.0;
        }

        // 计算冲突率
        double conflictRate() const
        {
            uint64_t total = hits + misses;
            return total > 0 ? static_cast<double>(conflicts) / total : 0.0;
        }
    };

    // 缓存基类
    class Cache
    {
    public:
        // 构造函数
        Cache(const CacheConfig &config, int id = 0, Bus *bus = nullptr);
        virtual ~Cache() = default;

        // 获取配置
        const CacheConfig &getConfig() const { return config_; }

        // 获取 ID
        int getId() const { return id_; }

        // 计算组索引
        size_t getSetIndex(uint64_t address) const;

        // 计算标签
        uint64_t getTag(uint64_t address) const;

        // 计算块内偏移
        size_t getBlockOffset(uint64_t address) const;

        // 查找缓存行
        CacheLine *findLine(uint64_t address);

        // 重置统计信息
        void resetStats();

        // 读取数据
        bool read(uint64_t address);

        // 写入数据
        bool write(uint64_t address, uint8_t value);

        // 嗅探总线请求
        // 返回 true 表示本地缓存拥有该数据块
        bool snoop(uint64_t address, BusEvent event);

        // 选择要替换的缓存行（由子类实现具体策略）
        virtual CacheLine *selectVictim(size_t set_index) = 0;

        // 更新访问信息（由子类实现）
        virtual void updateAccessInfo(size_t set_index, CacheLine *line) = 0;

        // 重置缓存行信息（当行被驱逐或重新分配时调用，由子类实现）
        virtual void resetLine(size_t set_index, CacheLine *) = 0;

        // 获取统计信息
        const CacheStats &getStats() const
        {
            return stats_;
        }

    protected:
        // 缓存配置
        CacheConfig config_;

        // 缓存 ID
        int id_;

        // 总线指针
        Bus *bus_;

        // 缓存统计信息
        CacheStats stats_;

        // 缓存组
        std::vector<CacheSet> sets_;
    };

} // namespace cache_sim

#endif // CACHE_H
