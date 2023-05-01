cmake_minimum_required(VERSION 3.0)
project( pub )

find_package(json-c CONFIG)
find_package(PkgConfig REQUIRED)
pkg_check_modules(Mosquitto IMPORTED_TARGET libmosquitto REQUIRED)

add_executable( pub Pub.cpp Subscriber.cpp )
target_link_libraries( pub PkgConfig::Mosquitto json-c::json-c)
