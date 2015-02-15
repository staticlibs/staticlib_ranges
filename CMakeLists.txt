
cmake_minimum_required ( VERSION 2.8.12 )

# toolchain setup
set ( STATICLIB_TOOLCHAIN linux_amd64_clang CACHE STRING "toolchain triplet" )
set ( CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/toolchains/${STATICLIB_TOOLCHAIN}.cmake CACHE INTERNAL "" )

# project setup
project ( movable_ranges CXX )
# common include
include ( ${CMAKE_CURRENT_LIST_DIR}/cmake/CMakeLists_common.cmake )

# build steps
include_directories ( ${CMAKE_CURRENT_LIST_DIR}/include )

# tests and docs
staticlib_enable_testing ( "" "" )
staticlib_enable_docs ( )
