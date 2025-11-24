cmake_minimum_required(VERSION 3.0)
project(CAN_Bridge CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(include)

file(GLOB SRC_FILES
    src/*.cpp
)

add_executable(can_bridge ${SRC_FILES})
target_link_libraries(can_bridge pthread)
