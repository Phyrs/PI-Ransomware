cmake_minimum_required(VERSION 2.6)

INCLUDE(${PROJECT_SOURCE_DIR}/Pin_sdk_version.txt)

#Rules for win or linux
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows" OR ${CMAKE_C_PLATFORM_ID} STREQUAL "MinGW")
	set(PIN_BUILD_FILE ${PIN_FILE_MSVC})
	set(PIN_HOME ${SYSTEMDRIVE}/${PIN_VERSION_MSVC})
elseif(${CMAKE_C_PLATFORM_ID} STREQUAL "Linux")
	set(PIN_BUILD_FILE ${PIN_FILE_LINUX})
	set(PIN_HOME ${PROJECT_BINARY_DIR}/${PIN_VERSION_LINUX})
endif()


#Download, extract pin and set PIN_HOME
if(NOT EXISTS ${PIN_HOME})
   include(pin_package/download-unpack.cmake)
endif(NOT EXISTS ${PIN_HOME})