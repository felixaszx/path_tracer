cmake_minimum_required(VERSION 3.20.0)
project(apcc VERSION 0.1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS off)

file(GLOB_RECURSE SRC src/*.cpp src/*.c)
add_executable(apcc ${SRC})
target_compile_options(apcc PUBLIC -pg -no-pie)

# includes-cpp
target_include_directories(apcc PUBLIC include)

# libraries-cpp
target_link_libraries(apcc PUBLIC -pg -no-pie)
target_link_libraries(apcc PUBLIC cglm)
