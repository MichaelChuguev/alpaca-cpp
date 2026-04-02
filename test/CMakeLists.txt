# ---------------------------------------------------------------------------
# GoogleTest — always build from source via FetchContent to guarantee ABI
# compatibility with the current compiler.
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(alpaca-tests
    models/test_account.cpp
    models/test_asset.cpp
    models/test_order.cpp
    models/test_position.cpp
    models/test_portfolio.cpp
    models/test_watchlist.cpp
    models/test_wallet.cpp
    models/test_activity.cpp
    models/test_calendar.cpp
    models/test_clock.cpp
    models/test_option.cpp
    models/test_treasury.cpp
    models/test_account_config.cpp
    models/test_error.cpp
    models/test_broker_account_trading_details.cpp
    models/test_broker_account_trading_limit.cpp
    models/test_broker_instant_funding.cpp
    models/test_stream_models.cpp
    trader/test_trader_api.cpp
    data/test_stock_data.cpp
    data/test_market_data_api.cpp
    data/test_option_data.cpp
    data/test_market_data_query_utils.cpp
    data/test_crypto_data.cpp
    data/test_news_data.cpp
)

target_include_directories(alpaca-tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    BEFORE SYSTEM PRIVATE
        ${googletest_SOURCE_DIR}/googletest/include
        ${googletest_SOURCE_DIR}/googlemock/include
)

target_link_libraries(alpaca-tests
    PRIVATE
        alpaca-cpp
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
)

gtest_discover_tests(alpaca-tests)
