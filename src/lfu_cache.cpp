#include "lfu_cache.h"
#include <chrono>

namespace cache_sim
{
    LFUCache::LFUCache(const CacheConfig &config, int id, Bus *bus)
        : Cache(config, id, bus)
    {
        lfu_sets_.resize(sets_.size());
    }

    CacheLine *LFUCache::selectVictim(size_t set_index)
    {
        CacheSet &set = sets_[set_index];

        // 首先查找无效的缓存行
        for (auto &line : set.lines)
        {
            if (!line.valid)
            {
                return &line;
            }
        }

        stats_.conflicts++;

        auto &lfu_set = lfu_sets_[set_index];

        if (lfu_set.freq_list.empty())
        {
            return &set.lines[0];
        }

        // 获取最小频率的列表
        auto &list = lfu_set.freq_list[lfu_set.min_freq];
        if (list.empty())
        {
            return &set.lines[0];
        }

        // 列表尾部是 LRU (最早访问的)
        return list.back();
    }

    void LFUCache::updateAccessInfo(size_t set_index, CacheLine *line)
    {
        if (line == nullptr)
            return;

        auto &lfu_set = lfu_sets_[set_index];

        // 更新时间戳
        line->last_access_time = static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());

        if (lfu_set.line_to_node.count(line))
        {
            // 缓存行已存在
            uint64_t old_freq = line->access_count;
            auto it = lfu_set.line_to_node[line];

            // 从旧频率列表中移除
            lfu_set.freq_list[old_freq].erase(it);

            // 如果旧频率列表为空且是最小频率，更新最小频率
            if (lfu_set.freq_list[old_freq].empty())
            {
                lfu_set.freq_list.erase(old_freq);
                if (lfu_set.min_freq == old_freq)
                {
                    lfu_set.min_freq++;
                }
            }

            // 更新频率
            line->access_count++;
            uint64_t new_freq = line->access_count;

            // 插入到新频率列表头部
            lfu_set.freq_list[new_freq].push_front(line);
            lfu_set.line_to_node[line] = lfu_set.freq_list[new_freq].begin();
        }
        else
        {
            // 新缓存行
            line->access_count = 1;
            uint64_t new_freq = 1;

            lfu_set.freq_list[new_freq].push_front(line);
            lfu_set.line_to_node[line] = lfu_set.freq_list[new_freq].begin();
            lfu_set.min_freq = 1;
        }
    }

    void LFUCache::resetLine(size_t set_index, CacheLine *line)
    {
        if (line == nullptr)
            return;

        auto &lfu_set = lfu_sets_[set_index];

        if (lfu_set.line_to_node.count(line))
        {
            uint64_t freq = line->access_count;
            auto it = lfu_set.line_to_node[line];

            lfu_set.freq_list[freq].erase(it);
            if (lfu_set.freq_list[freq].empty())
            {
                lfu_set.freq_list.erase(freq);
            }
            lfu_set.line_to_node.erase(line);
        }

        line->access_count = 0;
    }

} // namespace cache_sim
