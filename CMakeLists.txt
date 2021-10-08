cmake_minimum_required(VERSION 3.16)

# CHANGE YOUR PROJECT NAME
set(PROJECT_NAME c_string_tests)
project(${PROJECT_NAME} C CXX)

set(LIBN my_c_string)

# Options
set (WARNINGS_AS_ERRORS ON)
if (NOT ENABLE_PVS_STUDIO)
    set(ENABLE_PVS_STUDIO ON)
endif ()

#####################################################################################################
# 1) set up gtests. This block of code is taken from https://crascit.com/2015/07/25/cmake-gtest/
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
        )
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
        )

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This adds the following targets:
# gtest, gtest_main, gmock and gmock_main
add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
        "${CMAKE_BINARY_DIR}/googletest-build"
        )

#####################################################################################################

# 2) build my_str library
add_library(
        ${LIBN} SHARED
        ${CMAKE_SOURCE_DIR}/c_str_lib/c_string.c
        ${CMAKE_SOURCE_DIR}/c_str_lib/c_string.h
)
target_include_directories(${LIBN} PUBLIC ${CMAKE_SOURCE_DIR}/c_str_lib)

#####################################################################################################
# 3) build tests
add_executable(gtester ${CMAKE_SOURCE_DIR}/google_tests/main.cpp ${CMAKE_SOURCE_DIR}/google_tests/Tests/tests.cpp)
target_compile_definitions(gtester PUBLIC FILE_DIR="${CMAKE_SOURCE_DIR}/google_tests/test_files")
target_link_libraries(gtester ${LIBN} gtest gtest_main)

###################################
# set output directory (bin)
set_target_properties(${LIBN} gtester
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(${LIBN}
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib_bin
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib_bin)

set (ALL_TARGETS ${LIBN} gtester)
include(cmake/config.cmake)
