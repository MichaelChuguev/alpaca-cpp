cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

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
find_package(Boost 1.70 REQUIRED)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json 3.7.3 REQUIRED)

# Add library subdirectory
add_subdirectory(src)

# Add tests if enabled
if(ALPACA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# Add examples if enabled
if(ALPACA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

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