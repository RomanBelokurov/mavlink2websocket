cmake_minimum_required (VERSION 2.8.8)

project(uavClient)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O2")

find_package(Boost REQUIRED COMPONENTS system thread)

add_executable(${PROJECT_NAME} telemetry_service.cpp)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} pthread)