cmake_minimum_required(VERSION 3.15)
project(hb)

find_package(harfbuzz CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_executable(hb main.cpp)

target_link_libraries(hb harfbuzz::harfbuzz Freetype::Freetype GLEW::GLEW glfw)