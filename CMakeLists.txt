# CMake Version
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

# Project Name
PROJECT(RPiWebUI.Luna)

# Configuration
# Set Build Directories
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(
	./ 
	./dependency/Luna/src/Library 
	./dependency/Luna/src/FossLibrary
	./dependency/Luna/src/ThirdPartyLibrary
	./dependency/Luna/src/ProductLibrary)


# Check OS and Processor Information 
#
SET(CMAKE_CXX_FLAGS "-std=c++11")
ADD_DEFINITIONS(-Wall -DCALIB_CFG_OS="${CMAKE_SYSTEM_NAME}" -DCALIB_CFG_CPU="${CMAKE_SYSTEM_PROCESSOR}")

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#
# Make Libraries
#
ADD_SUBDIRECTORY(dependency/Luna)

file(GLOB SRC_FILES 
	src/*.cpp
	src/system/*.cpp
	src/fake_device/*.cpp
	src/md5/*.cpp
)

add_executable (RPiWebUI.Luna
    ${SRC_FILES}
)

TARGET_LINK_LIBRARIES(RPiWebUI.Luna
	ccMongooseServer
	ccJsonParser
	ccWebServer
	ccNetwork
	ccCore
	pthread
)  # RPiWebUI.Luna now depends on ccCore ... and ccMongooseServer

