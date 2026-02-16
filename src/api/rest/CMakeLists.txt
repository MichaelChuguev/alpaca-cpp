# src/api/rest/CMakeLists.txt

# Core library target
add_library(alpaca_api_rest STATIC
    HTTPClient.cpp
    # Add other core source files here
)

# Include directories
target_include_directories(alpaca_api_rest
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Dependencies
target_link_libraries(alpaca_api_rest
    PRIVATE
        nlohmann_json::nlohmann_json
        Boost::boost
)

# Compile features
target_compile_features(alpaca_api_rest
    PRIVATE 
        cxx_std_20
)

# Installation rules
install(TARGETS alpaca_core
    EXPORT alpaca_core_targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../include/alpaca/api/rest/
    DESTINATION include/alpaca/api/rest
)

# Optional: Testing section
if(BUILD_TESTING)
    add_subdirectory(test)  # If you have core tests
endif()