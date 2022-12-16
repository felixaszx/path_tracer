cmake_minimum_required(VERSION 3.10.0)
project(apcc VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS off)

file(GLOB_RECURSE SRC src/*.cpp src/*.c)
add_executable(apcc main.cpp ${SRC})
target_compile_options(apcc PUBLIC -pg -no-pie)

# includes-cpp
target_include_directories(apcc PUBLIC include)

# libraries-cpp
target_link_libraries(apcc PUBLIC -pg -no-pie)
target_link_libraries(apcc PUBLIC fmt)
