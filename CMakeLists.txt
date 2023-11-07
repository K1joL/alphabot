cmake_minimum_required(VERSION 3.0)
project( main )

find_package(PkgConfig REQUIRED)
pkg_check_modules(Mosquitto IMPORTED_TARGET libmosquitto libmosquittopp REQUIRED)

find_package(json-c CONFIG)

find_package( OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( main main.cpp detector.cpp mqtt.cpp controller.cpp )
target_link_libraries( main PkgConfig::Mosquitto json-c::json-c ${OpenCV_LIBS} )