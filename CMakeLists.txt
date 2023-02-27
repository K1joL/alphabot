cmake_minimum_required(VERSION 3.20)
project( main )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( main alphabot.cpp )
target_link_libraries( main ${OpenCV_LIBS} )