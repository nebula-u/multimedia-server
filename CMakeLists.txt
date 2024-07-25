cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_STANDARD 17)
project(multimedia-server)

include_directories(./include)

aux_source_directory(./src DIR_SRCS_MAIN)

add_executable(server ${DIR_SRCS_MAIN})

# 添加pthread库
find_package(Threads REQUIRED)
target_link_libraries(server Threads::Threads)
find_package(CURL REQUIRED)
target_link_libraries(server ${CURL_LIBRARIES})