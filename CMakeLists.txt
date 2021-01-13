cmake_minimum_required (VERSION 2.8.8)

project(uavClient)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O2")

# include glib and libsoup directories
include_directories(/usr/include/glib-2.0)
include_directories(/usr/include/libsoup-2.4)

# glibconfig.h directory
include_directories(/usr/lib/arm-linux-gnueabihf/glib-2.0/include/)

add_executable(${PROJECT_NAME} service_main.cpp)

target_link_libraries(${PROJECT_NAME} ${LIBSOUP_LIBRARIES})
target_link_libraries(${PROJECT_NAME} pthread soup-2.4 gio-2.0 gobject-2.0 glib-2.0)