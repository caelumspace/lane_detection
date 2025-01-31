cmake_minimum_required(VERSION 3.5)
project(LaneDetection)

find_package(OpenCV REQUIRED)

add_executable(LaneDetection main.cpp)
target_link_libraries(LaneDetection ${OpenCV_LIBS})
