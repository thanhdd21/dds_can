cmake_minimum_required(VERSION 3.10)
project(DDS_CAN_Bridge CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(
    include
    include/can
    include/dds
    src
)

# Find all DDS topic folders in src/idl
file(GLOB DDS_TOPIC_DIRS RELATIVE ${CMAKE_SOURCE_DIR}/src/idl ${CMAKE_SOURCE_DIR}/src/idl/*)

set(DDS_INCLUDE_DIRS "")
set(DDS_SRC_FILES "")

foreach(topic ${DDS_TOPIC_DIRS})
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/src/idl/${topic})
        # Include generated headers
        list(APPEND DDS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/idl/${topic})
        # Add generated source files
        file(GLOB topic_src ${CMAKE_SOURCE_DIR}/src/idl/${topic}/*.cxx)
        list(APPEND DDS_SRC_FILES ${topic_src})
    endif()
endforeach()

include_directories(${DDS_INCLUDE_DIRS})

# Your source files
file(GLOB SRC_FILES src/*.cpp src/*.cxx)

# Combine everything
set(ALL_SRC_FILES ${SRC_FILES} ${DDS_SRC_FILES})

# Create executable
add_executable(dds_can_bridge ${ALL_SRC_FILES})

# Find and link Fast DDS
#find_package(fastrtps REQUIRED)
find_package(fastcdr REQUIRED)
find_package(fastdds REQUIRED)

target_link_libraries(dds_can_bridge
    #fastrtps
    fastdds
    fastcdr
    pthread
)
