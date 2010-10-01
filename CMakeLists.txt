cmake_minimum_required (VERSION 2.6)
project (COMP_VISION)

add_definitions(-g -Wall -pedantic)

add_executable(pro-image image-main.cpp image-basic.cpp region-ops.cpp boundary-ops.cpp greylevel-ops.cpp eval.cpp statistic.cpp curvature.cpp)

target_link_libraries(pro-image tiff)

find_package(GTK2 2.6)

if(GTK2_FOUND)
      include_directories(${GTK2_INCLUDE_DIRS})
      add_executable(pro-gtk-image gtk-image.cpp gtk-main.cpp image-basic.cpp region-ops.cpp boundary-ops.cpp greylevel-ops.cpp eval.cpp statistic.cpp curvature.cpp)
      target_link_libraries(pro-gtk-image tiff ${GTK2_LIBRARIES})
endif()
