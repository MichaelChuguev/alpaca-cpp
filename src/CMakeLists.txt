add_library(alpaca-cpp
    alpacaAPI.cpp
    api/rest/HTTPClient.cpp
    api/websocket/WebSocketClient.cpp
    api/websocket/AlpacaUpdatesStream.cpp
    api/websocket/AlpacaMarketDataStream.cpp
    trader/AlpacaTraderAPI.cpp
    core/types.cpp
)


# Fixed include directories
target_include_directories(alpaca-cpp
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        # Expose IXWebSocket headers when consuming from the build tree.
        # ixwebsocket_SOURCE_DIR is set by FetchContent (or the system install
        # already puts headers on a system path, so this is a no-op then).
        $<BUILD_INTERFACE:${ixwebsocket_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(alpaca-cpp
    PUBLIC
        date::date
        OpenSSL::SSL
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
        # IXWebSocket drives both the HTTP (REST) and WebSocket layers.
        # System install exposes ixwebsocket::ixwebsocket; FetchContent exposes ixwebsocket.
        $<IF:$<TARGET_EXISTS:ixwebsocket::ixwebsocket>,ixwebsocket::ixwebsocket,ixwebsocket>
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