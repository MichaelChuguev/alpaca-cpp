cmake_minimum_required(VERSION 3.16)
project(alpaca-example VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option 1: If alpaca-cpp is installed system-wide
# find_package(alpaca-cpp 1.0.0 REQUIRED)

# Option 2: Use the build directory directly (for development)
set(alpaca-cpp_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../build")
find_package(alpaca-cpp REQUIRED)

# Create example executables
add_executable(simple_example simple_example.cpp)
target_link_libraries(simple_example PRIVATE alpaca::alpaca-cpp)

add_executable(trading_example trading_example.cpp)
target_link_libraries(trading_example PRIVATE alpaca::alpaca-cpp)

add_executable(watchlist_example watchlist_example.cpp)
target_link_libraries(watchlist_example PRIVATE alpaca::alpaca-cpp)

add_executable(crypto_example crypto_example.cpp)
target_link_libraries(crypto_example PRIVATE alpaca::alpaca-cpp)

add_executable(websocket_example websocket_example.cpp)
target_link_libraries(websocket_example PRIVATE alpaca::alpaca-cpp)

add_executable(stock_data_example stock_data_example.cpp)
target_link_libraries(stock_data_example PRIVATE alpaca::alpaca-cpp)

add_executable(option_data_example option_data_example.cpp)
target_link_libraries(option_data_example PRIVATE alpaca::alpaca-cpp)

add_executable(crypto_data_example crypto_data_example.cpp)
target_link_libraries(crypto_data_example PRIVATE alpaca::alpaca-cpp)

add_executable(news_screener_example news_screener_example.cpp)
target_link_libraries(news_screener_example PRIVATE alpaca::alpaca-cpp)
