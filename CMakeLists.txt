cmake_minimum_required(VERSION 3.15)
project(hb)

find_package(harfbuzz CONFIG REQUIRED)

add_executable(hb main.cpp)

target_link_libraries(hb harfbuzz::harfbuzz)