cmake_minimum_required(VERSION 3.29)
project(sylar)

set(CMAKE_CXX_STANDARD 20)

# 指定头文件  module/log
#include_directories(${PROJECT_SOURCE_DIR}/module)

# 指定包含目录
include_directories(
    module/log
    module/testVector
    module/getcwd_t
    module/TEMP

    module/signal_t
    module/exit_t
    module/SYSTEM_EXEC

    module/FORK_T
    module/zombie_process

    module/MULITI_PROCESS
    module/sharedMemory
    module/CircularQueue # 循环队列
    module/Semaphore # 信号量

    # 网络  相关 =========
    networkmodule/network # 线程
    networkmodule/client # 客户端
    networkmodule/networkMultiProcess # 多进程网络
    
    #发送文件
    networkmodule/file/clientFile
    networkmodule/file/serverFile


    #  select 模型 
    networkmodule/selectModel/clientselect
    networkmodule/selectModel/tcpselect


    # epoll 模型 
    networkmodule/epollModel/tcpepoll
    networkmodule/epollModel/tcpepoll_horizontal_edge_trigger # 边沿触发 水平触发

    
    networkmodule/epollModel/epoll_client # 边沿触发 水平触发

    /lib/print_t# 指定动态链接库的目录
)




file(GLOB_RECURSE MODULE  "module/*.*" "networkmodule/*.*")

file(GLOB_RECURSE SOURCES main.cpp)

add_executable(sylar ${SOURCES} ${MODULE})




# 动态链接库

# file(GLOB lib_SOURCES "lib/print_t/*.*")

# add_library(lib_SOURCES SHARED ${lib_SOURCES})

# # 设置库版本和输出名称 ; 设置目标的输出目录和版本信息
# set_target_properties(
#         lib_SOURCES
#         PROPERTIES
#         LIBRARY_OUTPUT_DIRECTORY
#         "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib"
# )


# #添加动态链接库 的include ;  包含目录，使得其他目标可以找到头文件
# target_include_directories(
#         sylar
#         PRIVATE
#         "${CMAKE_CURRENT_SOURCE_DIR}/lib/print_t/"
# )


# # 添加动态链接库到 主项目
# target_link_libraries(
#         sylar
#         PRIVATE
#         lib_SOURCES
# )



