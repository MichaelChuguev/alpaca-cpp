add_library(alpaca-cpp
    api/rest/HTTPClient.cpp
    auth/AlpacaAuthAPI.cpp
    api/websocket/WebSocketClient.cpp
    api/websocket/AlpacaUpdatesStream.cpp
    api/websocket/AlpacaMarketDataStream.cpp
    api/sse/BrokerSSEClient.cpp
    trader/AlpacaTraderAPI.cpp
    broker/AlpacaBrokerAPI.cpp
    marketdata/AlpacaMarketDataAPI.cpp
    core/types.cpp
)

if(TARGET ixwebsocket::ixwebsocket)
    set(ALPACA_IXWEBSOCKET_TARGET ixwebsocket::ixwebsocket)
elseif(TARGET ixwebsocket)
    set(ALPACA_IXWEBSOCKET_TARGET ixwebsocket)
else()
    message(FATAL_ERROR "IXWebSocket target not found")
endif()


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
    ${ALPACA_IXWEBSOCKET_TARGET}
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