cmake_minimum_required(VERSION 3.28)

project(LustLang LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CMakeDependentOption)
include(GenerateExportHeader)
include(CheckIncludeFile)
include_directories(
    AFTER
        ${CMAKE_BINARY_DIR}/export_headers
)

# Building library and executable in same directory to avoid "dll not found" problem in Windows
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/Binaries")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/Binaries")

# Set current root directory
set(LUST_CMAKE_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

# Load helper functions
include(cmake/TargetHelper.cmake)

# Add targets
add_subdirectory(thirdparty)
add_subdirectory(crates)

option(LUST_ENABLE_TESTS "" ON)
if (LUST_ENABLE_TESTS OR CMAKE_PROJECT_IS_TOP_LEVEL)
    enable_testing()
    add_subdirectory(tests)
endif()
