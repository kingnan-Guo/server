cmake_minimum_required(VERSION 3.29)
project(ReactorClient)

set(CMAKE_CXX_STANDARD 20)

# 指定头文件  module/log
#include_directories(${PROJECT_SOURCE_DIR}/module)

# 指定包含目录
include_directories(

    # 网络相关 =========

    module/client # 客户端
    module/epoll_client # epoll 客户端

    # 动态库 =========
    # /lib/print_t# 指定动态链接库的目录

)


file(GLOB_RECURSE MODULE  "module/*.*")

file(GLOB_RECURSE SOURCES main.cpp)

add_executable(ReactorClient ${SOURCES} ${MODULE})



