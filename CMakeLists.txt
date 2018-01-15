cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0048 NEW)
project (ScBridge VERSION 0.0.1)

message(STATUS "")
message(STATUS "ScBridge [${PROJECT_VERSION}] config init...")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

message(STATUS "CMAKE_GENERATOR : ${CMAKE_GENERATOR}")
message(STATUS "CMAKE_CONFIGURATION_TYPES : ${CMAKE_CONFIGURATION_TYPES}")

add_subdirectory (source)

message(STATUS "SCBRIDGE_VERSION : " ${PROJECT_VERSION} )



