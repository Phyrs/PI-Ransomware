cmake_minimum_required(VERSION 2.6)

# Change here the project name
project(PinBinaryTracer)

# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)

set (DOWNLOAD_PIN 1)

##Only build pintool on architectures (Linux or Windows)
if(DOWNLOAD_PIN AND "${CMAKE_C_PLATFORM_ID}" STREQUAL "Windows" OR "${CMAKE_C_PLATFORM_ID}" STREQUAL "Linux")
	set(DOWNLOAD_PIN 1)
endif()

if(DOWNLOAD_PIN)
	add_subdirectory (pin_sdk)
endif(DOWNLOAD_PIN)

add_subdirectory (src)
