#include <gtest/gtest.h>
#include "cache.h"

TEST(CacheTest, ParseAddress)
{
    using namespace cache_sim;

    CacheConfig config;
    config.cache_size = 1024; // 1KB
    config.block_size = 16;   // 16B
    config.associativity = 4; // 4 路组相联

    Cache cache(config);

    uint64_t address = 0x12345678;

    size_t set_index = cache.getSetIndex(address);
    uint64_t tag = cache.getTag(address);
    size_t block_offset = cache.getBlockOffset(address);

    EXPECT_EQ(set_index, 7);    // 测试 Set Index
    EXPECT_EQ(tag, 0x123456);   // 测试 Tag
    EXPECT_EQ(block_offset, 8); // 测试 Block Offset
}
