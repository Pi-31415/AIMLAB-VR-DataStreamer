################################################################################
# AIMLAB-VR-DataStreamer Tests CMake Configuration
#
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 21 October 2025
# Version: v1.0
#
# Description:
#   CMake configuration for test applications.
#
# Changelog:
#   v1.0 - 21 October 2025 - Initial creation
################################################################################

# Unit test for data packet
add_executable(test_datapacket test_datapacket.cpp)
target_link_libraries(test_datapacket PRIVATE aimlab_vr_datastreamer)

# Unit test for logger
add_executable(test_logger test_logger.cpp)
target_link_libraries(test_logger PRIVATE aimlab_vr_datastreamer)

# Integration test
add_executable(test_integration test_integration.cpp)
target_link_libraries(test_integration PRIVATE aimlab_vr_datastreamer)

