# Core library target
add_library(alpaca_trader STATIC
    AlpacaTraderAPI.cpp
    
    # Add other core source files here
)

# Include directories
target_include_directories(alpaca_trader
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Dependencies
target_link_libraries(alpaca_trader
    PRIVATE
        nlohmann_json::nlohmann_json
        Boost::boost
)

# Compile features
target_compile_features(alpaca_trader
    PRIVATE 
        cxx_std_20
)

# Installation rules
install(TARGETS alpaca_trader
    EXPORT alpaca_trader_targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../include/alpaca/model/trader/
    DESTINATION include/alpaca/trader
)

# Optional: Testing section
if(BUILD_TESTING)
    add_subdirectory(test)  # If you have core tests
endif()