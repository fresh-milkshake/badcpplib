cmake_minimum_required(VERSION 3.12)
project(BadCppLib VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project options
option(BADCPPLIB_BUILD_SHARED "Build shared library" OFF)
option(BADCPPLIB_BUILD_EXAMPLES "Build examples" ON)
option(BADCPPLIB_BUILD_TESTS "Build tests" OFF)

# Project directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Compiler settings
if(MSVC)
    add_compile_options(/W4 /permissive-)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od /Zi)
    else()
        add_compile_options(/O2)
    endif()
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O2)
    endif()
endif()

# Library source files
set(BADCPPLIB_SOURCES
    ${SRC_DIR}/badcpplib.cpp
    ${SRC_DIR}/pch.cpp
    ${SRC_DIR}/modules/test_framework.cpp
    ${SRC_DIR}/modules/string_utils.cpp
    ${SRC_DIR}/modules/math_utils.cpp
)

set(BADCPPLIB_HEADERS
    ${INCLUDE_DIR}/badcpplib.hpp
    ${INCLUDE_DIR}/framework.hpp
    ${INCLUDE_DIR}/pch.hpp
)

# Create library
if(BADCPPLIB_BUILD_SHARED)
    add_library(badcpplib SHARED ${BADCPPLIB_SOURCES})
    target_compile_definitions(badcpplib PRIVATE BADCPPLIB_EXPORTS)
else()
    add_library(badcpplib STATIC ${BADCPPLIB_SOURCES})
endif()

# Target setup
target_include_directories(badcpplib PUBLIC
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Set target properties
set_target_properties(badcpplib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME "badcpp"
)

# Create alias
add_library(BadCppLib::badcpplib ALIAS badcpplib)

# Examples
if(BADCPPLIB_BUILD_EXAMPLES)
    add_executable(example ${EXAMPLES_DIR}/example.cpp)
    target_link_libraries(example PRIVATE badcpplib)
    
    # Build into build directory
    set_target_properties(example PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# Tests (if any)
if(BADCPPLIB_BUILD_TESTS)
    enable_testing()
    if(EXISTS ${TESTS_DIR})
        add_subdirectory(${TESTS_DIR})
    endif()
endif()

# Install
include(GNUInstallDirs)

install(TARGETS badcpplib
    EXPORT BadCppLibTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES ${BADCPPLIB_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/badcpplib
)

# Export targets
install(EXPORT BadCppLibTargets
    FILE BadCppLibTargets.cmake
    NAMESPACE BadCppLib::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BadCppLib
)

# Generate config files
include(CMakePackageConfigHelpers)

# If configuration file exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/BadCppLibConfig.cmake.in")
    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/BadCppLibConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/BadCppLibConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BadCppLib
    )
endif()

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/BadCppLibConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/BadCppLibConfig.cmake")
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/BadCppLibConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BadCppLibConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BadCppLib
    )
endif()

# CPack for packaging
set(CPACK_PACKAGE_NAME "BadCppLib")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Comprehensive C++ utility library")
set(CPACK_PACKAGE_CONTACT "Developer")

include(CPack)

# Print build information
message(STATUS "")
message(STATUS "BadCppLib v${PROJECT_VERSION} Build Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Shared library: ${BADCPPLIB_BUILD_SHARED}")
message(STATUS "  Build examples: ${BADCPPLIB_BUILD_EXAMPLES}")
message(STATUS "  Build tests: ${BADCPPLIB_BUILD_TESTS}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Source directory: ${SRC_DIR}")
message(STATUS "  Include directory: ${INCLUDE_DIR}")
message(STATUS "  Examples directory: ${EXAMPLES_DIR}")
message(STATUS "")

# Create custom targets
if(BADCPPLIB_BUILD_EXAMPLES)
    add_custom_target(run
        COMMAND example
        DEPENDS example
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running example application"
    )
endif()

# Find clang-format
find_program(CLANG_FORMAT_EXECUTABLE clang-format)
if(CLANG_FORMAT_EXECUTABLE)
    file(GLOB_RECURSE ALL_SOURCE_FILES 
        ${SRC_DIR}/*.cpp 
        ${INCLUDE_DIR}/*.hpp 
        ${EXAMPLES_DIR}/*.cpp
    )
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${ALL_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting source code"
    )
endif()

add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMENT "Deep clean of build directory"
) 