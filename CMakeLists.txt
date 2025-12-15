cmake_minimum_required(VERSION 3.10)
project(OS-Cache-Simulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加 GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 添加编译选项
add_compile_options(-Wall -Wextra -Wpedantic -O2)

# 引入头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 源文件列表
set(SOURCES
    src/cache.cpp
    src/lru_cache.cpp
    src/lfu_cache.cpp
    src/cache_simulator.cpp
)

# 创建可执行文件
add_executable(cache_sim src/main.cpp ${SOURCES})

# 创建测试
enable_testing()

set(TESTS
    ${SOURCES}
    test/cache_test.cpp
)

add_executable(cache_sim_tests ${TESTS})
target_link_libraries(cache_sim_tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(cache_sim_tests)

