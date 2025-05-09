cmake_minimum_required(VERSION 3.10)
project(vaders-wrapper-firmware)

set(firmware_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/firmware)

enable_testing()
add_subdirectory(test)