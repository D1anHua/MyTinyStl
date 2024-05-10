cmake_minimum_required(VERSION 3.15.0)
project(DHS_TINY_STL)

SET(CMAKE_BUILD_TYPE Debug)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin/)

include_directories(${PROJECT_SOURCE_DIR}/DhsTinySTL)
add_executable(main ${PROJECT_SOURCE_DIR}/Test/main.cpp)