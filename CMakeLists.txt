cmake_minimum_required(VERSION 3.22)
project(visual_novel_video_generator VERSION 0.1.0 LANGUAGES C CXX)

# Standard Settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# GCC/Clang Release size optimization with GNU-style linker options
if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$"
        AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU"
        AND NOT APPLE)
    add_compile_options(
        "$<$<CONFIG:Release>:-Os>"
        "$<$<CONFIG:Release>:-flto>"
        "$<$<CONFIG:Release>:-ffunction-sections>"
        "$<$<CONFIG:Release>:-fdata-sections>"
    )

    add_link_options(
        "$<$<CONFIG:Release>:-flto>"
        "$<$<CONFIG:Release>:-Wl,--gc-sections>"
        "$<$<CONFIG:Release>:-Wl,-s>"
    )
endif()

# ------------------------------------------------------------------------------
# Dependencies (FetchContent)
# ------------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    zip
    GIT_REPOSITORY https://github.com/kuba--/zip.git
    GIT_TAG        v0.3.15
)
FetchContent_MakeAvailable(zip)

# ------------------------------------------------------------------------------
# Application Sources (src/)
# ------------------------------------------------------------------------------
file(GLOB_RECURSE APP_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE APP_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# ------------------------------------------------------------------------------
# Executable Definition
# ------------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
    ${APP_HEADERS}
)

# ------------------------------------------------------------------------------
# Include Directories
# ------------------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE zip::zip)

# Compile Flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wconversion)
endif()

# ------------------------------------------------------------------------------
# Unit Testing (doctest)
# ------------------------------------------------------------------------------
option(BUILD_TESTS "Build unit test suite" ON)
if(BUILD_TESTS)
    enable_testing()

    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG        v2.4.12
    )
    FetchContent_MakeAvailable(doctest)

    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

    set(TEST_IMPLEMENTATION_SOURCES ${APP_SOURCES})
    list(REMOVE_ITEM TEST_IMPLEMENTATION_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    )

    add_executable(test_suite
        ${TEST_SOURCES}
        ${TEST_IMPLEMENTATION_SOURCES}
    )

    target_include_directories(test_suite PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_link_libraries(test_suite PRIVATE zip::zip)

    target_compile_definitions(test_suite PRIVATE
        FINANCE_CORE_VERSION="${PROJECT_VERSION}"
    )

    target_link_libraries(test_suite PRIVATE doctest::doctest)

    include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
    doctest_discover_tests(test_suite)
endif()