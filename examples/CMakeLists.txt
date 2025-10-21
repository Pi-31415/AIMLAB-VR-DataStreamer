################################################################################
# AIMLAB-VR-DataStreamer Examples CMake Configuration
#
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 21 October 2025
# Version: v1.0
#
# Description:
#   CMake configuration for example applications.
#
# Changelog:
#   v1.0 - 21 October 2025 - Initial creation
################################################################################

# Basic streaming example
add_executable(basic_streaming_example basic_streaming_example.cpp)
target_link_libraries(basic_streaming_example PRIVATE aimlab_vr_datastreamer)

# Advanced streaming example
add_executable(advanced_streaming_example advanced_streaming_example.cpp)
target_link_libraries(advanced_streaming_example PRIVATE aimlab_vr_datastreamer)

# Network receiver example
add_executable(network_receiver_example network_receiver_example.cpp)
target_link_libraries(network_receiver_example PRIVATE aimlab_vr_datastreamer)

