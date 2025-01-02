cmake_minimum_required(VERSION 3.29)
project(Reactor)

set(CMAKE_CXX_STANDARD 20)

# 指定头文件  module/log
#include_directories(${PROJECT_SOURCE_DIR}/module)

# 指定包含目录
include_directories(

    # 组件 ===========
    components/InetAddress   #  地址
    components/Socket        # 套接字
    components/Epoll        # epoll 封装
    components/Channel     # channel 类
    components/EventLoop    # 事件循环 EventLoop 类
    components/TcpServer    # 封装 TcpServer 类
    components/Acceptor     # Acceptor 类
    components/Connection   # Connection 类
    components/Buffer       # Buffer 类 , 数据缓冲区

    

    # 网络相关 =========
    module/netserver # 线程
    module/netserver2 # 线程
    module/netserver3 # 线程


    # 动态库 =========
    # /lib/print_t# 指定动态链接库的目录

)


file(GLOB_RECURSE MODULE  "module/*.*" "components/*.*" )

file(GLOB_RECURSE SOURCES main.cpp)

add_executable(Reactor ${SOURCES} ${MODULE})




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



