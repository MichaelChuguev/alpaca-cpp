#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "nlohmann/json.hpp"

namespace alpaca {

using json = nlohmann::json;

class AlpacaException : public std::runtime_error {
public:
    enum class Kind {
        api,
        http,
        transport,
    };

    explicit AlpacaException(const json& j)
        : AlpacaException(Kind::api, std::nullopt, j.dump(), std::optional<json>(j)) {}

    AlpacaException(int http_status, const json& j)
        : AlpacaException(Kind::api, http_status, j.dump(), std::optional<json>(j)) {}

    [[nodiscard]] Kind kind() const noexcept { return kind_; }
    [[nodiscard]] std::optional<int> http_status() const noexcept { return http_status_; }
    [[nodiscard]] std::optional<std::string_view> code() const noexcept {
        if (!code_) {
            return std::nullopt;
        }
        return std::string_view(*code_);
    }
    [[nodiscard]] std::string_view message() const noexcept { return what(); }
    [[nodiscard]] const std::string& raw_body() const noexcept { return raw_body_; }
    [[nodiscard]] const std::optional<json>& body() const noexcept { return body_; }

    [[nodiscard]] const json* field(std::string_view key) const {
        if (!body_) {
            return nullptr;
        }

        const auto it = body_->find(std::string(key));
        return it == body_->end() ? nullptr : &*it;
    }

    template <typename T>
    [[nodiscard]] std::optional<T> field_as(std::string_view key) const {
        const auto* value = field(key);
        if (!value || value->is_null()) {
            return std::nullopt;
        }

        try {
            return value->get<T>();
        } catch (const json::exception&) {
            return std::nullopt;
        }
    }

    [[nodiscard]] static bool is_error_payload(const json& j) noexcept {
        return j.is_object() &&
               (j.contains("code") || j.contains("message") || j.contains("msg"));
    }

    [[nodiscard]] static AlpacaException from_http_response(
        int http_status,
        const std::string& response_body) {
        const auto body = parse_body(response_body);
        const auto kind =
            body && is_error_payload(*body) ? Kind::api : Kind::http;
        return AlpacaException(kind, http_status, response_body, body);
    }

    [[nodiscard]] static AlpacaException from_transport_error(
        std::string message,
        std::string raw_body = {}) {
        return AlpacaException(
            Kind::transport, std::move(message), std::nullopt, std::move(raw_body), std::nullopt);
    }

private:
    AlpacaException(Kind kind,
                    std::optional<int> http_status,
                    std::string raw_body,
                    std::optional<json> body)
        : AlpacaException(kind,
                          build_message(kind, http_status, raw_body, body),
                          std::move(http_status),
                          std::move(raw_body),
                          std::move(body)) {}

    AlpacaException(Kind kind,
                    std::string message,
                    std::optional<int> http_status,
                    std::string raw_body,
                    std::optional<json> body)
        : std::runtime_error(std::move(message)),
          kind_(kind),
          http_status_(http_status),
          code_(extract_code(body)),
          raw_body_(raw_body.empty() && body ? body->dump() : std::move(raw_body)),
          body_(std::move(body)) {}

    static std::optional<json> parse_body(const std::string& response_body) {
        if (response_body.empty()) {
            return std::nullopt;
        }

        try {
            return json::parse(response_body);
        } catch (const json::exception&) {
            return std::nullopt;
        }
    }

    static std::string stringify_value(const json& value) {
        if (value.is_string()) {
            return value.get<std::string>();
        }
        return value.dump();
    }

    static std::string build_http_message(const std::optional<int>& http_status,
                                          const std::string& response_body) {
        const std::string prefix =
            http_status ? "HTTP " + std::to_string(*http_status) : "HTTP error";
        if (response_body.empty()) {
            return prefix;
        }
        return prefix + ": " + response_body;
    }

    static std::string extract_message(const json& j) {
        if (j.contains("message") && !j["message"].is_null()) {
            return stringify_value(j["message"]);
        }
        if (j.contains("msg") && !j["msg"].is_null()) {
            return stringify_value(j["msg"]);
        }
        return "Unknown API error";
    }

    static std::string build_message(Kind kind,
                                     const std::optional<int>& http_status,
                                     const std::string& raw_body,
                                     const std::optional<json>& body) {
        if (kind == Kind::api && body) {
            return extract_message(*body);
        }
        return build_http_message(http_status, raw_body);
    }

    static std::optional<std::string> extract_code(const std::optional<json>& body) {
        if (!body || !body->contains("code") || (*body)["code"].is_null()) {
            return std::nullopt;
        }
        return stringify_value((*body)["code"]);
    }

    Kind kind_ = Kind::api;
    std::optional<int> http_status_;
    std::optional<std::string> code_;
    std::string raw_body_;
    std::optional<json> body_;
};

using AlpacaTraderException = AlpacaException;

} // namespace alpaca
