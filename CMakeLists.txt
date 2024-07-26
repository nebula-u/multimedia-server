cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_STANDARD 17)
project(multimedia-server)

include_directories(./include)
include_directories(./plugin/jsoncpp/json)
include_directories((./plugin/threadpool))

aux_source_directory(./src DIR_SRCS_MAIN)
aux_source_directory(./plugin/jsoncpp DIR_SRCS_JSON)
aux_source_directory(./plugin/threadpool DIR_SRCS_THREADPOOL)

add_executable(server ${DIR_SRCS_MAIN} ${DIR_SRCS_JSON} ${DIR_SRCS_THREADPOOL})

# 添加pthread库
find_package(Threads REQUIRED)
target_link_libraries(server Threads::Threads)
find_package(CURL REQUIRED)
target_link_libraries(server ${CURL_LIBRARIES})