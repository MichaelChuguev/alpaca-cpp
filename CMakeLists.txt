cmake_minimum_required(VERSION 3.16)

project(alpaca-cpp 
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "C++ client library for Alpaca API"
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project options
option(ALPACA_BUILD_TESTS "Build tests" ON)
option(ALPACA_BUILD_EXAMPLES "Build examples" OFF)

# Find dependencies
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json 3.7.3 REQUIRED)
find_package(date REQUIRED)

# ---------------------------------------------------------------------------
# IXWebSocket — used for both HTTP (REST) and WebSocket layers.
# Prefer a system install, fall back to FetchContent.
# ---------------------------------------------------------------------------
include(FetchContent)

set(ALPACA_USE_BUNDLED_IXWEBSOCKET OFF)

find_package(ixwebsocket QUIET)
if(NOT ixwebsocket_FOUND)
    message(STATUS "IXWebSocket not found via find_package – fetching from GitHub")
    # Use OpenSSL (already found above) as the TLS backend.
    set(USE_TLS             ON  CACHE BOOL "" FORCE)
    set(USE_OPEN_SSL        ON  CACHE BOOL "" FORCE)
    set(USE_ZLIB            OFF CACHE BOOL "" FORCE)
    set(IXWEBSOCKET_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        ixwebsocket
        URL https://github.com/machinezone/IXWebSocket/archive/refs/tags/v11.4.4.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(ixwebsocket)
    set(ALPACA_USE_BUNDLED_IXWEBSOCKET ON)
endif()

# Add library subdirectory
add_subdirectory(src)

# Add tests if enabled
if(ALPACA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# Add examples if enabled
if(ALPACA_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

# Installation configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# If IXWebSocket was bundled via FetchContent, install it as part of this package
# so consumers can resolve transitive includes and linking from the same prefix.
if(ALPACA_USE_BUNDLED_IXWEBSOCKET AND TARGET ixwebsocket)
    # IXWebSocket sets PUBLIC_HEADER entries relative to its own source tree,
    # which can trigger install() warnings in this parent project.
    set_target_properties(ixwebsocket PROPERTIES PUBLIC_HEADER "")

    install(TARGETS ixwebsocket
        EXPORT alpaca-cpp-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    if(DEFINED ixwebsocket_SOURCE_DIR AND EXISTS "${ixwebsocket_SOURCE_DIR}/ixwebsocket")
        install(DIRECTORY "${ixwebsocket_SOURCE_DIR}/ixwebsocket/"
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ixwebsocket
            FILES_MATCHING PATTERN "*.h"
        )
    endif()
endif()

# Generate package configuration files
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/alpaca-cppConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/alpaca-cppConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/alpaca-cpp
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/alpaca-cppConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install config files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/alpaca-cppConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/alpaca-cppConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/alpaca-cpp
)

# Export targets for build tree (allows using library without installing)
export(EXPORT alpaca-cpp-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/alpaca-cppTargets.cmake
    NAMESPACE alpaca::
)