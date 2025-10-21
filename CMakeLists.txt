################################################################################
# AIMLAB-VR-DataStreamer CMake Build Configuration
#
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 21 October 2025
# Version: v1.0
#
# Description:
#   Cross-platform CMake configuration for AIMLAB VR Data Streamer.
#   Supports Windows, Linux, and macOS builds.
#
# Changelog:
#   v1.0 - 21 October 2025 - Initial creation
################################################################################

cmake_minimum_required(VERSION 3.15)

project(AIMLAB_VR_DataStreamer
    VERSION 1.0.0
    DESCRIPTION "Platform agnostic VR Data Streamer for AIMLAB Experiments"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Build options
option(BUILD_EXAMPLES "Build example applications" ON)
option(BUILD_TESTS "Build test applications" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# Platform-specific settings
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)  # Windows 7 or later
    add_definitions(-DPLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    add_definitions(-DPLATFORM_LINUX)
elseif(APPLE)
    add_definitions(-DPLATFORM_MACOS)
endif()

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

# Source files
set(CORE_SOURCES
    src/VRDataStreamer.cpp
    src/DataPacket.cpp
    src/NetworkManager.cpp
    src/VRDevice.cpp
    src/Logger.cpp
    src/Config.cpp
)

set(CORE_HEADERS
    include/aimlab-vr-datastreamer/VRDataStreamer.h
    include/aimlab-vr-datastreamer/DataPacket.h
    include/aimlab-vr-datastreamer/NetworkManager.h
    include/aimlab-vr-datastreamer/VRDevice.h
    include/aimlab-vr-datastreamer/Logger.h
    include/aimlab-vr-datastreamer/Config.h
    include/aimlab-vr-datastreamer/Types.h
)

# Main library
add_library(aimlab_vr_datastreamer ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(aimlab_vr_datastreamer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Platform-specific linking
if(WIN32)
    target_link_libraries(aimlab_vr_datastreamer PRIVATE ws2_32)
elseif(UNIX)
    target_link_libraries(aimlab_vr_datastreamer PRIVATE pthread)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS aimlab_vr_datastreamer
    EXPORT AIMLAB_VR_DataStreamerTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/aimlab-vr-datastreamer
    DESTINATION include
)

# Export configuration
install(EXPORT AIMLAB_VR_DataStreamerTargets
    FILE AIMLAB_VR_DataStreamerTargets.cmake
    NAMESPACE AIMLAB_VR::
    DESTINATION lib/cmake/AIMLAB_VR_DataStreamer
)

