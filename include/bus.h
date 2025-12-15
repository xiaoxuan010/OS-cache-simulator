#ifndef BUS_H
#define BUS_H

#include <bits/stdc++.h>

namespace cache_sim
{

    class Cache;

    // 总线事件类型
    enum class BusEvent
    {
        BusRd,  // 读请求：请求读取数据块，不打算修改
        BusRdX, // 独占读请求：请求读取数据块，打算修改
    };

    // 总线类，负责连接所有缓存并广播请求
    class Bus
    {
    public:
        // 将缓存连接到总线
        void attach(Cache *cache);

        // 广播总线请求
        // sender_id: 发起请求的缓存ID
        // address: 请求的地址
        // event: 请求类型
        // 返回 true 表示有其他缓存拥有该数据
        bool broadcast(int sender_id, uint64_t address, BusEvent event);

    private:
        std::vector<Cache *> caches_;
    };

} // namespace cache_sim

#endif // BUS_H
