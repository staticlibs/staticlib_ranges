
cmake_minimum_required ( VERSION 2.8.3 )

# toolchain setup
set ( STATICLIB_TOOLCHAIN amd64_linux_clang CACHE STRING "toolchain triplet" )
set ( CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/toolchains/toolchain-${STATICLIB_TOOLCHAIN}.cmake CACHE INTERNAL "" )

# project setup
project ( movable_ranges CXX )
include ( ${CMAKE_CURRENT_LIST_DIR}/cmake/CMakeLists_common.cmake )

# output dirs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# build steps
include_directories ( ${CMAKE_CURRENT_LIST_DIR}/include )

# tests and docs
staticlib_enable_testing ( "" )
staticlib_enable_docs ( )
