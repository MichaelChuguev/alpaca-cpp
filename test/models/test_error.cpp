#include <gtest/gtest.h>

#include <cstdint>
#include <type_traits>

#include "alpaca/core/error.h"

using namespace alpaca;

static_assert(std::is_same_v<AlpacaException, AlpacaTraderException>,
              "AlpacaTraderException should remain a compatibility alias");

namespace {

[[noreturn]] void throw_like_http_client(int http_status, const std::string& response_body) {
    throw AlpacaException::from_http_response(http_status, response_body);
}

[[noreturn]] void throw_like_transport(const std::string& message,
                                       const std::string& response_body = {}) {
    throw AlpacaException::from_transport_error(message, response_body);
}

} // namespace

TEST(AlpacaExceptionTest, ConstructFromJsonErrorPayload) {
    json j = {
        {"code", 40310000},
        {"message", "no available position for the specified contract"}
    };

    AlpacaException ex(403, j);

    EXPECT_EQ(ex.kind(), AlpacaException::Kind::api);
    ASSERT_TRUE(ex.http_status().has_value());
    EXPECT_EQ(*ex.http_status(), 403);
    ASSERT_TRUE(ex.code().has_value());
    EXPECT_EQ(*ex.code(), "40310000");
    EXPECT_STREQ(ex.what(), "no available position for the specified contract");
    ASSERT_TRUE(ex.body().has_value());
    EXPECT_EQ((*ex.body())["code"], 40310000);
    ASSERT_TRUE(ex.field_as<std::int64_t>("code").has_value());
    EXPECT_EQ(*ex.field_as<std::int64_t>("code"), 40310000);
}

TEST(AlpacaExceptionTest, SupportsLegacyAliasCatchType) {
    json j = {
        {"code", 40310000},
        {"message", "cannot close an account with open positions or non-zero balance"}
    };

    try {
        throw AlpacaException(403, j);
    } catch (const AlpacaTraderException& ex) {
        ASSERT_TRUE(ex.http_status().has_value());
        EXPECT_EQ(*ex.http_status(), 403);
        ASSERT_TRUE(ex.code().has_value());
        EXPECT_EQ(*ex.code(), "40310000");
        EXPECT_STREQ(ex.what(), "cannot close an account with open positions or non-zero balance");
        return;
    }

    FAIL() << "Expected AlpacaTraderException compatibility alias to catch AlpacaException";
}

TEST(AlpacaExceptionTest, FromHttpResponseParsesMessagePayload) {
    const std::string payload =
        R"({"code":40310000,"message":"no available position for the specified contract"})";
    const auto ex = AlpacaException::from_http_response(403, payload);

    EXPECT_EQ(ex.kind(), AlpacaException::Kind::api);
    ASSERT_TRUE(ex.http_status().has_value());
    EXPECT_EQ(*ex.http_status(), 403);
    ASSERT_TRUE(ex.code().has_value());
    EXPECT_EQ(*ex.code(), "40310000");
    EXPECT_STREQ(ex.what(), "no available position for the specified contract");
    EXPECT_EQ(ex.raw_body(), payload);
    ASSERT_TRUE(ex.body().has_value());
    EXPECT_EQ(ex.body()->at("code"), 40310000);
}

TEST(AlpacaExceptionTest, FromHttpResponseParsesMsgPayload) {
    const auto ex = AlpacaException::from_http_response(
        406,
        R"({"code":406,"msg":"connection limit exceeded"})");

    EXPECT_EQ(ex.kind(), AlpacaException::Kind::api);
    ASSERT_TRUE(ex.http_status().has_value());
    EXPECT_EQ(*ex.http_status(), 406);
    ASSERT_TRUE(ex.code().has_value());
    EXPECT_EQ(*ex.code(), "406");
    EXPECT_STREQ(ex.what(), "connection limit exceeded");
}

TEST(AlpacaExceptionTest, FromHttpResponsePreservesVariableFieldTypes) {
    const auto ex = AlpacaException::from_http_response(
        422,
        R"({"code":{"category":"orders","id":7},"message":{"text":"order rejected"},"retryable":true,"details":{"field":"qty"},"attempts":[1,2]})");

    EXPECT_EQ(ex.kind(), AlpacaException::Kind::api);
    ASSERT_TRUE(ex.code().has_value());
    EXPECT_EQ(*ex.code(), R"({"category":"orders","id":7})");
    EXPECT_EQ(ex.message(), R"({"text":"order rejected"})");
    ASSERT_TRUE(ex.field_as<bool>("retryable").has_value());
    EXPECT_TRUE(*ex.field_as<bool>("retryable"));
    const auto* details = ex.field("details");
    ASSERT_NE(details, nullptr);
    EXPECT_TRUE(details->is_object());
    const auto* attempts = ex.field("attempts");
    ASSERT_NE(attempts, nullptr);
    EXPECT_TRUE(attempts->is_array());
}

TEST(AlpacaExceptionTest, FromHttpResponsePreservesUnrelatedJsonPayloadAsHttpError) {
    const auto ex = AlpacaException::from_http_response(
        500,
        R"({"foo":"bar"})");

    EXPECT_EQ(ex.kind(), AlpacaException::Kind::http);
    ASSERT_TRUE(ex.http_status().has_value());
    EXPECT_EQ(*ex.http_status(), 500);
    EXPECT_EQ(ex.message(), R"(HTTP 500: {"foo":"bar"})");
    EXPECT_FALSE(ex.code().has_value());
    ASSERT_TRUE(ex.body().has_value());
    EXPECT_EQ(ex.body()->at("foo"), "bar");
}

TEST(AlpacaExceptionTest, FromHttpResponsePreservesNonJsonPayloadAsHttpError) {
    const auto ex = AlpacaException::from_http_response(
        502,
        "<html>bad gateway</html>");

    EXPECT_EQ(ex.kind(), AlpacaException::Kind::http);
    ASSERT_TRUE(ex.http_status().has_value());
    EXPECT_EQ(*ex.http_status(), 502);
    EXPECT_EQ(ex.message(), "HTTP 502: <html>bad gateway</html>");
    EXPECT_EQ(ex.raw_body(), "<html>bad gateway</html>");
    EXPECT_FALSE(ex.body().has_value());
    EXPECT_FALSE(ex.code().has_value());
}

TEST(AlpacaExceptionTest, HttpClientStyleErrorTranslationThrowsTypedException) {
    try {
        throw_like_http_client(
            403,
            R"({"code":40310000,"message":"cannot close an account with open positions or non-zero balance"})");
    } catch (const AlpacaException& ex) {
        EXPECT_EQ(ex.kind(), AlpacaException::Kind::api);
        ASSERT_TRUE(ex.http_status().has_value());
        EXPECT_EQ(*ex.http_status(), 403);
        ASSERT_TRUE(ex.code().has_value());
        EXPECT_EQ(*ex.code(), "40310000");
        EXPECT_STREQ(ex.what(), "cannot close an account with open positions or non-zero balance");
        return;
    }

    FAIL() << "Expected AlpacaException for Alpaca-style JSON error payload";
}

TEST(AlpacaExceptionTest, HttpClientStyleNonJsonErrorStillThrowsTypedException) {
    try {
        throw_like_http_client(503, "service unavailable");
    } catch (const AlpacaException& ex) {
        EXPECT_EQ(ex.kind(), AlpacaException::Kind::http);
        ASSERT_TRUE(ex.http_status().has_value());
        EXPECT_EQ(*ex.http_status(), 503);
        EXPECT_EQ(ex.message(), "HTTP 503: service unavailable");
        return;
    }

    FAIL() << "Expected AlpacaException for non-JSON HTTP payload";
}

TEST(AlpacaExceptionTest, TransportErrorUsesUniformExceptionType) {
    try {
        throw_like_transport("HTTP transport error: connection timed out", "upstream timeout");
    } catch (const AlpacaException& ex) {
        EXPECT_EQ(ex.kind(), AlpacaException::Kind::transport);
        EXPECT_FALSE(ex.http_status().has_value());
        EXPECT_FALSE(ex.code().has_value());
        EXPECT_EQ(ex.message(), "HTTP transport error: connection timed out");
        EXPECT_EQ(ex.raw_body(), "upstream timeout");
        EXPECT_FALSE(ex.body().has_value());
        return;
    }

    FAIL() << "Expected AlpacaException for transport errors";
}
