
cmake_minimum_required ( VERSION 2.8 )

# common staticlibs setup
if ( NOT DEFINED STATICLIB_PARENT_DIR )
    set ( STATICLIB_TOOLCHAIN amd64_linux_clang CACHE STRING "toolchain triplet" )
    set ( STATICLIB_PARENT_DIR ${CMAKE_CURRENT_LIST_DIR}/../ CACHE INTERNAL "parent dir" )
    include ( ${STATICLIB_PARENT_DIR}/staticlib_util_cmake/CMakeLists_common.cmake )
    staticlib_set_toolchain ( ${STATICLIB_PARENT_DIR} ${STATICLIB_TOOLCHAIN} )
endif ( )

# project header
project ( staticlib_iterators CXX )
set ( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set ( ${PROJECT_NAME}_VERSION_MINOR 1 )
set ( ${PROJECT_NAME}_VERSION_MICRO 0 )

# build steps
include_directories ( ${CMAKE_CURRENT_LIST_DIR}/include )

# tests
staticlib_enable_testing ( "" )
staticlib_enable_docs ( )

# headers export
set ( ${PROJECT_NAME}_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include" CACHE INTERNAL ${PROJECT_NAME}_INCLUDES )
