#include <gtest/gtest.h>
#include "lru_cache.h"

using namespace cache_sim;

// 基础缓存地址解析测试
TEST(BaseCache, ParseAddress)
{
    CacheConfig config;
    config.cache_size = 1024; // 1KB
    config.block_size = 16;   // 16B
    config.associativity = 4; // 4 路组相联

    LRUCache cache(config);

    uint64_t address = 0x12345678;

    size_t set_index = cache.getSetIndex(address);
    uint64_t tag = cache.getTag(address);
    size_t block_offset = cache.getBlockOffset(address);

    EXPECT_EQ(set_index, 7);    // 测试 Set Index
    EXPECT_EQ(tag, 0x123456);   // 测试 Tag
    EXPECT_EQ(block_offset, 8); // 测试 Block Offset
}

// LRU 缓存读写基本功能测试
TEST(LRUCache, ReadWrite_Basic)
{
    CacheConfig config;
    config.cache_size = 1024; // 1KB
    config.block_size = 16;   // 16B
    config.associativity = 4; // 4 路组相联

    LRUCache cache(config);

    uint64_t A = 0x1000;

    // 初始读取，应该是缺失
    EXPECT_FALSE(cache.read(A));

    // 再次读取，应该是命中
    EXPECT_TRUE(cache.read(A));

    uint64_t B = 0x2000;

    // 写入数据
    EXPECT_FALSE(cache.write(B, 0xAB));

    // 读取写入的数据，应该是命中
    EXPECT_TRUE(cache.read(B));
}

// LRU 替换策略基础测试
TEST(LRUCache, LRUReplacement)
{
    CacheConfig config(512, 16, 2);
    LRUCache cache(config);

    // 都映射到同一组（index = 0）
    uint32_t A = 0x0000; // index 0
    uint32_t B = 0x0100; // index 0
    uint32_t C = 0x0200; // index 0

    EXPECT_FALSE(cache.read(A)); // 未命中，插入A
    EXPECT_TRUE(cache.read(A));  // 命中
    EXPECT_FALSE(cache.read(B)); // 未命中，插入B（组已满：A,B）
    EXPECT_FALSE(cache.read(C)); // 未命中 -> 淘汰LRU（A），插入C

    EXPECT_FALSE(cache.read(A)); // A已被淘汰 -> 未命中
    EXPECT_TRUE(cache.read(C));  // C应在缓存中 -> 命中
    EXPECT_TRUE(cache.read(A));  // A应仍在缓存中 -> 命中
}
