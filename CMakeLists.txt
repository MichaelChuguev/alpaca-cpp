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
endif()

# Add examples if enabled
if(ALPACA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation configuration
include(GNUInstallDirs)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})