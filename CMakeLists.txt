CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

# Global VARIABLES
set(CMAKE_PROJECT_NAME "obstacle_avoidance")
set(CMAKE_CXX_FLAGS "-std=c++11")

# Set some CMake Project values correctly
PROJECT(${CMAKE_PROJECT_NAME})

# Find OpenCV
find_package(OpenCV REQUIRED)

# Set our sources
set(SRCS src/main.cpp)

# Some information for the build
MESSAGE(STATUS "Current built type: ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "Current build type flags: ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")

# And build
add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})