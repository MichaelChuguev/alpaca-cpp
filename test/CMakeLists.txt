find_package(GTest REQUIRED
    PATHS /opt/homebrew/lib/cmake
    NO_DEFAULT_PATH
)

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
    trader/test_trader_api.cpp
)

target_include_directories(alpaca-tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(alpaca-tests
    PRIVATE
        alpaca-cpp
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        nlohmann_json::nlohmann_json
        Boost::boost
        OpenSSL::SSL
        OpenSSL::Crypto
)

gtest_discover_tests(alpaca-tests)
