cmake_minimum_required(VERSION 3.15) # Or a newer version
project(MyHarfBuzzProject)

# Find HarfBuzz using CMake's find_package
find_package(harfbuzz CONFIG REQUIRED)

# Add your executable (replace main.cpp with your source files)
add_executable(MyHarfBuzzExecutable main.cpp)

# Link against the HarfBuzz library
target_link_libraries(MyHarfBuzzExecutable harfbuzz::harfbuzz)