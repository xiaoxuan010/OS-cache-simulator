#include "lru_cache.h"
#include <chrono>

namespace cache_sim
{

    LRUCache::LRUCache(const CacheConfig &config, int id, Bus *bus)
        : Cache(config, id, bus)
    {
        lru_sets_.resize(sets_.size());
    }

    CacheLine *LRUCache::selectVictim(size_t set_index)
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
        
        // 使用 LRU 双向链表查找
        auto &lru_set = lru_sets_[set_index];
        if (!lru_set.lru_list.empty())
        {
            // 返回最久未使用的缓存行
            return lru_set.lru_list.back();
        }

        return &set.lines[0];
    }

    void LRUCache::updateAccessInfo(size_t set_index, CacheLine *line)
    {
        if (line == nullptr) return;

        line->last_access_time = std::chrono::steady_clock::now().time_since_epoch().count();

        auto &lru_set = lru_sets_[set_index];
        auto it = lru_set.line_to_node.find(line);

        if (it != lru_set.line_to_node.end())
        {
            // 已经在列表中，移动到头部 (MRU)
            lru_set.lru_list.splice(lru_set.lru_list.begin(), lru_set.lru_list, it->second);
        }
        else
        {
            // 不在列表中，插入到头部
            lru_set.lru_list.push_front(line);
            lru_set.line_to_node[line] = lru_set.lru_list.begin();
        }
    }

    void LRUCache::resetLine(size_t set_index, CacheLine *line)
    {
        if (line == nullptr) return;

        auto &lru_set = lru_sets_[set_index];
        auto it = lru_set.line_to_node.find(line);

        if (it != lru_set.line_to_node.end())
        {
            // 从 LRU 列表中移除
            lru_set.lru_list.erase(it->second);
            lru_set.line_to_node.erase(it);
        }
    }

} // namespace cache_sim
