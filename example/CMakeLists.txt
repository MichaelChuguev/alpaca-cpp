cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# When built as a subdirectory from the root project, the alpaca-cpp target is
# already available.  When built standalone (e.g. after an install), use
# find_package instead.
if(NOT TARGET alpaca-cpp)
    project(alpaca-examples VERSION 1.0.0 LANGUAGES CXX)
    find_package(alpaca-cpp REQUIRED)
endif()

set(EXAMPLES
    simple_example
    trading_example
    watchlist_example
    crypto_example
    websocket_example
    stock_data_example
    option_data_example
    crypto_data_example
    news_screener_example
    broker_sse_example
    broker_get_accounts_example
)

foreach(example ${EXAMPLES})
    add_executable(${example} ${example}.cpp)
    target_link_libraries(${example} PRIVATE alpaca::alpaca-cpp)
endforeach()
