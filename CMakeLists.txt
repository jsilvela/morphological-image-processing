cmake_minimum_required (VERSION 2.6)
project (COMP_VISION)

file(GLOB BASE_CPP "*.cpp")
file(GLOB GTK_CPP "gtk*cpp")
file(GLOB MAIN_CPP "*main*cpp")
file(GLOB MAIN2_CPP "*compare*cpp")
list(REMOVE_ITEM BASE_CPP ${GTK_CPP} ${MAIN_CPP} ${MAIN2_CPP})

add_definitions(-g -Wall -pedantic)
add_executable(pro-image image-main.cpp ${BASE_CPP})
target_link_libraries(pro-image tiff)

add_executable(compare-pro-image compare.cpp ${BASE_CPP})
target_link_libraries(compare-pro-image tiff)

cmake_policy(SET CMP0011 NEW)

find_package(TIFF 2.6 REQUIRED)
find_package(GTK2 2.6)
if(GTK2_FOUND)
      include_directories(${GTK2_INCLUDE_DIRS})
      add_executable(pro-gtk-image ${GTK_CPP} ${BASE_CPP})
      target_link_libraries(pro-gtk-image tiff ${GTK2_LIBRARIES})
endif()
