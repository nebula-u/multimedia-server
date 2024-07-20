cmake_minimum_required(VERSION 3.5.0)

project(multimedia-server)

include_directories(./include)

aux_source_directory(./src DIR_SRCS_MAIN)

add_executable(server ${DIR_SRCS_MAIN})