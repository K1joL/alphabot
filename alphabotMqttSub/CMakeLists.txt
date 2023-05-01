cmake_minimum_required(VERSION 3.0)
project( MqttSub )

find_package(json-c CONFIG)
find_package(PkgConfig REQUIRED)
pkg_check_modules(Mosquitto IMPORTED_TARGET libmosquitto REQUIRED)

add_executable( MqttSub MqttSub.cpp Subscriber.cpp )
target_link_libraries( MqttSub PkgConfig::Mosquitto json-c::json-c)
