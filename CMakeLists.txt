cmake_minimum_required(VERSION 3.10)
project(cache-sim VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
add_compile_options(-Wall -Wextra -Wpedantic -O2)

# 引入头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 源文件列表
set(SOURCES
    src/main.cpp
)

# 创建可执行文件
add_executable(cache-sim ${SOURCES})
