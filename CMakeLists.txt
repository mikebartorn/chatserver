cmake_minimum_required(VERSION 2.8)
project(chat)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#配置最终的可执行文件的输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#配置头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/database)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)
include_directories(/usr/include/mysql)

#加载子目录
add_subdirectory(src)

# #设置需要编译的源文件列表
# set(SRC_LIST ./muduo_server.cpp)
# #把指定文件下的所有源文件名称加入到变量名SRC_LIST中
# #aux_source_directory(. SRC_LIST)

# #表示可执行生成文件server由哪些源文件编译而来
# add_executable(server ${SRC_LIST})
# #表示server这个目标程序，需要连接这三个库文件
# target_link_libraries(server muduo_net muduo_base pthread)
