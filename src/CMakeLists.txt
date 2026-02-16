add_library(alpaca-cpp
    alpacaAPI.cpp
    api/rest/HTTPClient.cpp
    trader/AlpacaTraderAPI.cpp
    core/types.cpp
)


# Fixed include directories
target_include_directories(alpaca-cpp
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(alpaca-cpp
    PUBLIC
        Boost::boost
        CURL::libcurl
        OpenSSL::SSL
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
)

# Installation
# Install library
install(TARGETS alpaca-cpp
    EXPORT alpaca-cpp-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install export
install(EXPORT alpaca-cpp-targets
    FILE alpaca-cppTargets.cmake
    NAMESPACE alpaca::
    DESTINATION lib/cmake/alpaca-cpp
)