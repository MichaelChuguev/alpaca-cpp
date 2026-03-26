#include <gtest/gtest.h>
#include <iostream>
#include "nlohmann/json.hpp"
#include "alpaca/AlpacaMarketDataStream.h"
#include "alpaca/core/types.h"
#include "alpaca/model/stream/market_data.h"
#include "alpaca/model/stream/trade_update.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

class TestOptionMarketDataStream : public AlpacaOptionMarketDataStream {
public:
    using AlpacaOptionMarketDataStream::AlpacaOptionMarketDataStream;
    using AlpacaOptionMarketDataStream::on_raw_message;
};

} // namespace

// ===== StreamTrade Tests =====

TEST(StreamTradeTest, ParseFromJson) {
    std::cout << "\n=== Test: StreamTradeTest::ParseFromJson ===" << std::endl;
    std::cout << "Testing: WebSocket market data stream - Parse trade message" << std::endl;

    json j;
    j["T"] = "t";
    j["S"] = "AAPL";
    j["p"] = 185.50;
    j["s"] = 100;
    j["t"] = "2024-01-15T14:30:00Z";
    j["x"] = "V";
    j["i"] = 12345;
    j["z"] = "C";

    auto trade = StreamTrade::from_json(j);

    std::cout << "Response:" << std::endl;
    std::cout << "  Symbol: " << trade.symbol << std::endl;
    std::cout << "  Price: " << trade.price << std::endl;
    std::cout << "  Size: " << trade.size << std::endl;
    std::cout << "  Exchange: " << trade.exchange << std::endl;
    std::cout << "  Trade ID: " << trade.id << std::endl;
    std::cout << "  Tape: " << trade.tape << std::endl;

    EXPECT_EQ(trade.symbol, "AAPL");
    EXPECT_EQ(trade.price, Decimal(185.50));
    EXPECT_EQ(trade.size, Decimal(100));
    EXPECT_EQ(trade.exchange, "V");
    EXPECT_EQ(trade.id, 12345u);
    EXPECT_EQ(trade.tape, "C");

    std::cout << "PASSED" << std::endl;
}

TEST(StreamTradeTest, ParseMissingOptionalFields) {
    std::cout << "\n=== Test: StreamTradeTest::ParseMissingOptionalFields ===" << std::endl;
    std::cout << "Testing: StreamTrade with missing optional fields" << std::endl;

    json j;
    j["T"] = "t";
    j["S"] = "MSFT";
    j["p"] = 400.0;
    j["s"] = 50;

    auto trade = StreamTrade::from_json(j);

    std::cout << "Response: symbol=" << trade.symbol << ", price=" << trade.price
              << ", id=" << trade.id << std::endl;

    EXPECT_EQ(trade.symbol, "MSFT");
    EXPECT_EQ(trade.price, Decimal(400.0));
    EXPECT_EQ(trade.id, 0u);

    std::cout << "PASSED" << std::endl;
}

TEST(StreamTradeTest, ParseFromStreamArray) {
    std::cout << "\n=== Test: StreamTradeTest::ParseFromStreamArray ===" << std::endl;
    std::cout << "Testing: WebSocket message array with multiple trades" << std::endl;

    json msg = json::array();
    json t1;
    t1["T"] = "t";
    t1["S"] = "AAPL";
    t1["p"] = 185.50;
    t1["s"] = 100;
    msg.push_back(t1);

    json t2;
    t2["T"] = "t";
    t2["S"] = "MSFT";
    t2["p"] = 400.25;
    t2["s"] = 50;
    msg.push_back(t2);

    std::vector<StreamTrade> trades;
    for (const auto& item : msg) {
        if (item.contains("T") && item["T"] == "t") {
            trades.push_back(StreamTrade::from_json(item));
        }
    }

    std::cout << "Response: " << trades.size() << " trades" << std::endl;
    for (const auto& t : trades) {
        std::cout << "  " << t.symbol << " @ " << t.price << " x" << t.size << std::endl;
    }

    ASSERT_EQ(trades.size(), 2u);
    EXPECT_EQ(trades[0].symbol, "AAPL");
    EXPECT_EQ(trades[1].symbol, "MSFT");

    std::cout << "PASSED" << std::endl;
}

// ===== StreamQuote Tests =====

TEST(StreamQuoteTest, ParseFromJson) {
    std::cout << "\n=== Test: StreamQuoteTest::ParseFromJson ===" << std::endl;
    std::cout << "Testing: WebSocket market data stream - Parse quote message" << std::endl;

    json j;
    j["T"] = "q";
    j["S"] = "AAPL";
    j["bp"] = 185.40;
    j["bs"] = 200;
    j["bx"] = "Q";
    j["ap"] = 185.50;
    j["as"] = 150;
    j["ax"] = "P";
    j["t"] = "2024-01-15T14:30:00Z";
    j["z"] = "C";
    j["c"] = json::array({"R", "W"});

    auto quote = StreamQuote::from_json(j);

    std::cout << "Response:" << std::endl;
    std::cout << "  Symbol: " << quote.symbol << std::endl;
    std::cout << "  Bid: " << quote.bid_price << " x" << quote.bid_size << " (" << quote.bid_exchange << ")" << std::endl;
    std::cout << "  Ask: " << quote.ask_price << " x" << quote.ask_size << " (" << quote.ask_exchange << ")" << std::endl;
    std::cout << "  Tape: " << quote.tape << std::endl;
    std::cout << "  Conditions: ";
    for (const auto& c : quote.conditions) std::cout << c << " ";
    std::cout << std::endl;

    EXPECT_EQ(quote.symbol, "AAPL");
    EXPECT_EQ(quote.bid_price, Decimal(185.40));
    EXPECT_EQ(quote.bid_size, Decimal(200));
    EXPECT_EQ(quote.bid_exchange, "Q");
    EXPECT_EQ(quote.ask_price, Decimal(185.50));
    EXPECT_EQ(quote.ask_size, Decimal(150));
    EXPECT_EQ(quote.ask_exchange, "P");
    EXPECT_EQ(quote.tape, "C");
    ASSERT_EQ(quote.conditions.size(), 2u);
    EXPECT_EQ(quote.conditions[0], "R");
    EXPECT_EQ(quote.conditions[1], "W");

    std::cout << "PASSED" << std::endl;
}

TEST(StreamQuoteTest, ParseNoConditions) {
    std::cout << "\n=== Test: StreamQuoteTest::ParseNoConditions ===" << std::endl;
    std::cout << "Testing: StreamQuote without conditions array" << std::endl;

    json j;
    j["T"] = "q";
    j["S"] = "TSLA";
    j["bp"] = 250.0;
    j["bs"] = 100;
    j["bx"] = "N";
    j["ap"] = 250.10;
    j["as"] = 80;
    j["ax"] = "N";
    j["t"] = "2024-01-15T14:30:00Z";
    j["z"] = "C";

    auto quote = StreamQuote::from_json(j);

    std::cout << "Response: " << quote.symbol << " bid=" << quote.bid_price
              << " ask=" << quote.ask_price << " conditions=[]" << std::endl;

    EXPECT_EQ(quote.symbol, "TSLA");
    EXPECT_TRUE(quote.conditions.empty());

    std::cout << "PASSED" << std::endl;
}

TEST(StreamQuoteTest, ParseSpread) {
    std::cout << "\n=== Test: StreamQuoteTest::ParseSpread ===" << std::endl;
    std::cout << "Testing: Quote spread calculation from stream data" << std::endl;

    json j;
    j["T"] = "q";
    j["S"] = "SPY";
    j["bp"] = 475.50;
    j["bs"] = 500;
    j["bx"] = "Q";
    j["ap"] = 475.51;
    j["as"] = 300;
    j["ax"] = "P";
    j["t"] = "2024-01-15T14:30:00Z";
    j["z"] = "B";

    auto quote = StreamQuote::from_json(j);
    Decimal spread = quote.ask_price - quote.bid_price;

    std::cout << "Response: " << quote.symbol << " spread=$" << spread << std::endl;

    EXPECT_EQ(quote.symbol, "SPY");
    EXPECT_NEAR(spread.to_double(), 0.01, 0.001);

    std::cout << "PASSED" << std::endl;
}

// ===== StreamBar Tests =====

TEST(StreamBarTest, ParseFromJson) {
    std::cout << "\n=== Test: StreamBarTest::ParseFromJson ===" << std::endl;
    std::cout << "Testing: WebSocket market data stream - Parse bar message" << std::endl;

    json j;
    j["T"] = "b";
    j["S"] = "AAPL";
    j["o"] = 185.00;
    j["h"] = 186.50;
    j["l"] = 184.80;
    j["c"] = 186.20;
    j["v"] = 1500000;
    j["t"] = "2024-01-15T14:30:00Z";
    j["n"] = 5000;
    j["vw"] = 185.75;

    auto bar = StreamBar::from_json(j);

    std::cout << "Response:" << std::endl;
    std::cout << "  Symbol: " << bar.symbol << std::endl;
    std::cout << "  OHLC: " << bar.open << "/" << bar.high << "/" << bar.low << "/" << bar.close << std::endl;
    std::cout << "  Volume: " << bar.volume << std::endl;
    std::cout << "  Trade Count: " << bar.trade_count << std::endl;
    std::cout << "  VWAP: " << bar.vwap << std::endl;

    EXPECT_EQ(bar.symbol, "AAPL");
    EXPECT_EQ(bar.open, Decimal(185.00));
    EXPECT_EQ(bar.high, Decimal(186.50));
    EXPECT_EQ(bar.low, Decimal(184.80));
    EXPECT_EQ(bar.close, Decimal(186.20));
    EXPECT_EQ(bar.volume, Decimal(1500000));
    EXPECT_EQ(bar.trade_count, 5000u);
    EXPECT_EQ(bar.vwap, Decimal(185.75));

    std::cout << "PASSED" << std::endl;
}

TEST(StreamBarTest, ParseMissingTradeCount) {
    std::cout << "\n=== Test: StreamBarTest::ParseMissingTradeCount ===" << std::endl;
    std::cout << "Testing: StreamBar with missing trade count defaults to 0" << std::endl;

    json j;
    j["T"] = "b";
    j["S"] = "GOOGL";
    j["o"] = 140.0;
    j["h"] = 142.0;
    j["l"] = 139.5;
    j["c"] = 141.5;
    j["v"] = 500000;

    auto bar = StreamBar::from_json(j);

    std::cout << "Response: " << bar.symbol << " OHLC=" << bar.open << "/"
              << bar.high << "/" << bar.low << "/" << bar.close
              << " trade_count=" << bar.trade_count << std::endl;

    EXPECT_EQ(bar.symbol, "GOOGL");
    EXPECT_EQ(bar.trade_count, 0u);
    EXPECT_EQ(bar.volume, Decimal(500000));

    std::cout << "PASSED" << std::endl;
}

// ===== Mixed Stream Message Tests =====

TEST(StreamMessageTest, ParseMixedMessageArray) {
    std::cout << "\n=== Test: StreamMessageTest::ParseMixedMessageArray ===" << std::endl;
    std::cout << "Testing: WebSocket message with mixed trade/quote/bar messages" << std::endl;

    json msg = json::array();

    json trade;
    trade["T"] = "t";
    trade["S"] = "AAPL";
    trade["p"] = 185.50;
    trade["s"] = 100;
    msg.push_back(trade);

    json quote;
    quote["T"] = "q";
    quote["S"] = "AAPL";
    quote["bp"] = 185.40;
    quote["bs"] = 200;
    quote["bx"] = "Q";
    quote["ap"] = 185.50;
    quote["as"] = 150;
    quote["ax"] = "P";
    msg.push_back(quote);

    json bar;
    bar["T"] = "b";
    bar["S"] = "AAPL";
    bar["o"] = 185.00;
    bar["h"] = 186.00;
    bar["l"] = 184.50;
    bar["c"] = 185.50;
    bar["v"] = 1000000;
    msg.push_back(bar);

    std::vector<StreamTrade> trades;
    std::vector<StreamQuote> quotes;
    std::vector<StreamBar> bars;

    for (const auto& item : msg) {
        std::string type = item["T"].get<std::string>();
        if (type == "t") trades.push_back(StreamTrade::from_json(item));
        else if (type == "q") quotes.push_back(StreamQuote::from_json(item));
        else if (type == "b") bars.push_back(StreamBar::from_json(item));
    }

    std::cout << "Response: " << trades.size() << " trades, " << quotes.size()
              << " quotes, " << bars.size() << " bars" << std::endl;

    ASSERT_EQ(trades.size(), 1u);
    ASSERT_EQ(quotes.size(), 1u);
    ASSERT_EQ(bars.size(), 1u);
    EXPECT_EQ(trades[0].symbol, "AAPL");
    EXPECT_EQ(quotes[0].symbol, "AAPL");
    EXPECT_EQ(bars[0].symbol, "AAPL");

    std::cout << "PASSED" << std::endl;
}

// ===== Option Stream Model / Protocol Tests =====

TEST(StreamOptionTradeTest, ParseFromJson) {
    std::cout << "\n=== Test: StreamOptionTradeTest::ParseFromJson ===" << std::endl;
    std::cout << "Testing: Real-time option trade model parsing" << std::endl;

    json j;
    j["T"] = "t";
    j["S"] = "AAPL240315C00172500";
    j["t"] = "2024-03-11T13:35:35.13312256Z";
    j["p"] = 2.84;
    j["s"] = 1;
    j["x"] = "N";
    j["c"] = "S";

    auto trade = StreamOptionTrade::from_json(j);

    std::cout << "Response: " << trade.symbol << " @ " << trade.price
              << " x" << trade.size << " condition=" << trade.condition << std::endl;

    EXPECT_EQ(trade.symbol, "AAPL240315C00172500");
    EXPECT_EQ(trade.price, Decimal(2.84));
    EXPECT_EQ(trade.size, Decimal(1));
    EXPECT_EQ(trade.exchange, "N");
    EXPECT_EQ(trade.condition, "S");

    std::cout << "PASSED" << std::endl;
}

TEST(StreamOptionQuoteTest, ParseFromJson) {
    std::cout << "\n=== Test: StreamOptionQuoteTest::ParseFromJson ===" << std::endl;
    std::cout << "Testing: Real-time option quote model parsing" << std::endl;

    json j;
    j["T"] = "q";
    j["S"] = "SPXW240327P04925000";
    j["t"] = "2024-03-12T11:59:38.897261568Z";
    j["bx"] = "C";
    j["bp"] = 9.46;
    j["bs"] = 53;
    j["ax"] = "C";
    j["ap"] = 9.66;
    j["as"] = 38;
    j["c"] = "A";

    auto quote = StreamOptionQuote::from_json(j);

    std::cout << "Response: " << quote.symbol << " bid=" << quote.bid_price
              << " ask=" << quote.ask_price << " condition=" << quote.condition << std::endl;

    EXPECT_EQ(quote.symbol, "SPXW240327P04925000");
    EXPECT_EQ(quote.bid_price, Decimal(9.46));
    EXPECT_EQ(quote.bid_size, Decimal(53));
    EXPECT_EQ(quote.bid_exchange, "C");
    EXPECT_EQ(quote.ask_price, Decimal(9.66));
    EXPECT_EQ(quote.ask_size, Decimal(38));
    EXPECT_EQ(quote.ask_exchange, "C");
    EXPECT_EQ(quote.condition, "A");

    std::cout << "PASSED" << std::endl;
}

TEST(StreamOptionProtocolTest, ParseMsgpackTradeFrame) {
    std::cout << "\n=== Test: StreamOptionProtocolTest::ParseMsgpackTradeFrame ===" << std::endl;
    std::cout << "Testing: Option stream msgpack trade dispatch" << std::endl;

    TestOptionMarketDataStream stream("key", "secret");
    std::vector<StreamOptionTrade> trades;
    stream.on_trade([&](const StreamOptionTrade& trade) {
        trades.push_back(trade);
    });

    json frame = json::array({
        {
            {"T", "t"},
            {"S", "AAPL240315C00172500"},
            {"t", "2024-03-11T13:35:35.13312256Z"},
            {"p", 2.84},
            {"s", 1},
            {"x", "N"},
            {"c", "S"}
        }
    });

    const auto payload = json::to_msgpack(frame);
    stream.on_raw_message(std::string(payload.begin(), payload.end()));

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].symbol, "AAPL240315C00172500");
    EXPECT_EQ(trades[0].price, Decimal(2.84));
    EXPECT_EQ(trades[0].condition, "S");

    std::cout << "PASSED" << std::endl;
}

TEST(StreamOptionProtocolTest, ParseMsgpackQuoteFrame) {
    std::cout << "\n=== Test: StreamOptionProtocolTest::ParseMsgpackQuoteFrame ===" << std::endl;
    std::cout << "Testing: Option stream msgpack quote dispatch" << std::endl;

    TestOptionMarketDataStream stream("key", "secret");
    std::vector<StreamOptionQuote> quotes;
    stream.on_quote([&](const StreamOptionQuote& quote) {
        quotes.push_back(quote);
    });

    json frame = json::array({
        {
            {"T", "q"},
            {"S", "SPXW240327P04925000"},
            {"t", "2024-03-12T11:59:38.897261568Z"},
            {"bx", "C"},
            {"bp", 9.46},
            {"bs", 53},
            {"ax", "C"},
            {"ap", 9.66},
            {"as", 38},
            {"c", "A"}
        }
    });

    const auto payload = json::to_msgpack(frame);
    stream.on_raw_message(std::string(payload.begin(), payload.end()));

    ASSERT_EQ(quotes.size(), 1u);
    EXPECT_EQ(quotes[0].symbol, "SPXW240327P04925000");
    EXPECT_EQ(quotes[0].bid_price, Decimal(9.46));
    EXPECT_EQ(quotes[0].ask_price, Decimal(9.66));
    EXPECT_EQ(quotes[0].condition, "A");

    std::cout << "PASSED" << std::endl;
}

// ===== WebSocket Protocol Message Tests =====

TEST(StreamProtocolTest, AuthenticationMessage) {
    std::cout << "\n=== Test: StreamProtocolTest::AuthenticationMessage ===" << std::endl;
    std::cout << "Testing: WebSocket auth message construction" << std::endl;

    json auth;
    auth["action"] = "auth";
    auth["key"] = "APCA-API-KEY-ID";
    auth["secret"] = "APCA-API-SECRET-KEY";

    std::cout << "Auth message: " << auth.dump(2) << std::endl;

    EXPECT_EQ(auth["action"], "auth");
    EXPECT_TRUE(auth.contains("key"));
    EXPECT_TRUE(auth.contains("secret"));

    std::cout << "PASSED" << std::endl;
}

TEST(StreamProtocolTest, SubscribeMessage) {
    std::cout << "\n=== Test: StreamProtocolTest::SubscribeMessage ===" << std::endl;
    std::cout << "Testing: WebSocket subscribe message construction" << std::endl;

    json sub;
    sub["action"] = "subscribe";
    sub["trades"] = {"AAPL", "MSFT"};
    sub["quotes"] = {"AAPL"};
    sub["bars"] = {"SPY"};

    std::cout << "Subscribe message: " << sub.dump(2) << std::endl;

    EXPECT_EQ(sub["action"], "subscribe");
    ASSERT_EQ(sub["trades"].size(), 2u);
    EXPECT_EQ(sub["trades"][0], "AAPL");
    ASSERT_EQ(sub["quotes"].size(), 1u);
    ASSERT_EQ(sub["bars"].size(), 1u);

    std::cout << "PASSED" << std::endl;
}

TEST(StreamProtocolTest, UnsubscribeMessage) {
    std::cout << "\n=== Test: StreamProtocolTest::UnsubscribeMessage ===" << std::endl;
    std::cout << "Testing: WebSocket unsubscribe message construction" << std::endl;

    json unsub;
    unsub["action"] = "unsubscribe";
    unsub["trades"] = {"AAPL"};
    unsub["quotes"] = json::array();
    unsub["bars"] = json::array();

    std::cout << "Unsubscribe message: " << unsub.dump(2) << std::endl;

    EXPECT_EQ(unsub["action"], "unsubscribe");
    ASSERT_EQ(unsub["trades"].size(), 1u);

    std::cout << "PASSED" << std::endl;
}

TEST(StreamProtocolTest, SubscriptionConfirmation) {
    std::cout << "\n=== Test: StreamProtocolTest::SubscriptionConfirmation ===" << std::endl;
    std::cout << "Testing: Parse subscription confirmation from server" << std::endl;

    json confirmation = json::array();
    json msg;
    msg["T"] = "subscription";
    msg["trades"] = {"AAPL", "MSFT"};
    msg["quotes"] = {"AAPL"};
    msg["bars"] = {"SPY"};
    confirmation.push_back(msg);

    std::cout << "Confirmation: " << confirmation.dump(2) << std::endl;

    ASSERT_EQ(confirmation.size(), 1u);
    EXPECT_EQ(confirmation[0]["T"], "subscription");
    ASSERT_EQ(confirmation[0]["trades"].size(), 2u);

    std::cout << "PASSED" << std::endl;
}

TEST(StreamProtocolTest, AuthSuccessResponse) {
    std::cout << "\n=== Test: StreamProtocolTest::AuthSuccessResponse ===" << std::endl;
    std::cout << "Testing: Parse authentication success from server" << std::endl;

    json response = json::array();
    json msg;
    msg["T"] = "success";
    msg["msg"] = "authenticated";
    response.push_back(msg);

    std::cout << "Response: " << response.dump(2) << std::endl;

    EXPECT_EQ(response[0]["T"], "success");
    EXPECT_EQ(response[0]["msg"], "authenticated");

    std::cout << "PASSED" << std::endl;
}

TEST(StreamProtocolTest, AuthErrorResponse) {
    std::cout << "\n=== Test: StreamProtocolTest::AuthErrorResponse ===" << std::endl;
    std::cout << "Testing: Parse authentication error from server" << std::endl;

    json response = json::array();
    json msg;
    msg["T"] = "error";
    msg["code"] = 402;
    msg["msg"] = "auth failed";
    response.push_back(msg);

    std::cout << "Response: " << response.dump(2) << std::endl;

    EXPECT_EQ(response[0]["T"], "error");
    EXPECT_EQ(response[0]["code"], 402);
    EXPECT_EQ(response[0]["msg"], "auth failed");

    std::cout << "PASSED" << std::endl;
}

// ===== TradeUpdate Tests =====

TEST(TradeUpdateTest, ParseFillEvent) {
    std::cout << "\n=== Test: TradeUpdateTest::ParseFillEvent ===" << std::endl;
    std::cout << "Testing: WebSocket trade_updates - Parse fill event" << std::endl;

    json order_json;
    order_json["id"] = "order-abc-123";
    order_json["client_order_id"] = "client-123";
    order_json["symbol"] = "AAPL";
    order_json["asset_class"] = "us_equity";
    order_json["qty"] = "10";
    order_json["filled_qty"] = "10";
    order_json["side"] = "buy";
    order_json["type"] = "market";
    order_json["time_in_force"] = "day";
    order_json["status"] = "filled";
    order_json["created_at"] = "2024-01-15T14:30:00Z";
    order_json["submitted_at"] = "2024-01-15T14:30:00Z";
    order_json["filled_at"] = "2024-01-15T14:30:01Z";

    json j;
    j["event"] = "fill";
    j["execution_id"] = "exec-789";
    j["price"] = "185.50";
    j["qty"] = "10";
    j["position_qty"] = "10";
    j["timestamp"] = "2024-01-15T14:30:01Z";
    j["order"] = order_json;

    auto tu = TradeUpdate::from_json(j);

    std::cout << "Response:" << std::endl;
    std::cout << "  Event: " << trade_update_event_to_string(tu.event) << std::endl;
    std::cout << "  Execution ID: " << tu.execution_id << std::endl;
    std::cout << "  Price: " << tu.price << std::endl;
    std::cout << "  Qty: " << tu.qty << std::endl;
    std::cout << "  Position Qty: " << tu.position_qty << std::endl;
    if (tu.order) {
        std::cout << "  Order: " << tu.order->symbol << " (" << tu.order->id << ")" << std::endl;
    }

    EXPECT_EQ(tu.event, TradeUpdateEvent::FILL);
    EXPECT_EQ(tu.execution_id, "exec-789");
    EXPECT_EQ(tu.price, Decimal(185.50));
    EXPECT_EQ(tu.qty, Decimal(10.0));
    EXPECT_EQ(tu.position_qty, Decimal(10.0));
    EXPECT_TRUE(tu.order.has_value());
    EXPECT_EQ(tu.order->symbol, "AAPL");

    std::cout << "PASSED" << std::endl;
}

TEST(TradeUpdateTest, ParsePartialFillEvent) {
    std::cout << "\n=== Test: TradeUpdateTest::ParsePartialFillEvent ===" << std::endl;
    std::cout << "Testing: WebSocket trade_updates - Parse partial_fill event" << std::endl;

    json j;
    j["event"] = "partial_fill";
    j["execution_id"] = "exec-456";
    j["price"] = "185.25";
    j["qty"] = "5";
    j["position_qty"] = "5";
    j["timestamp"] = "2024-01-15T14:30:01Z";

    auto tu = TradeUpdate::from_json(j);

    std::cout << "Response: event=" << trade_update_event_to_string(tu.event)
              << ", qty=" << tu.qty << ", position_qty=" << tu.position_qty << std::endl;

    EXPECT_EQ(tu.event, TradeUpdateEvent::PARTIAL_FILL);
    EXPECT_EQ(tu.qty, Decimal(5.0));
    EXPECT_FALSE(tu.order.has_value());

    std::cout << "PASSED" << std::endl;
}

TEST(TradeUpdateTest, ParseCanceledEvent) {
    std::cout << "\n=== Test: TradeUpdateTest::ParseCanceledEvent ===" << std::endl;
    std::cout << "Testing: WebSocket trade_updates - Parse canceled event" << std::endl;

    json j;
    j["event"] = "canceled";
    j["execution_id"] = "";
    j["timestamp"] = "2024-01-15T14:35:00Z";

    auto tu = TradeUpdate::from_json(j);

    std::cout << "Response: event=" << trade_update_event_to_string(tu.event) << std::endl;

    EXPECT_EQ(tu.event, TradeUpdateEvent::CANCELED);

    std::cout << "PASSED" << std::endl;
}

TEST(TradeUpdateTest, ParseNewEvent) {
    std::cout << "\n=== Test: TradeUpdateTest::ParseNewEvent ===" << std::endl;
    std::cout << "Testing: WebSocket trade_updates - Parse new order event" << std::endl;

    json j;
    j["event"] = "new";
    j["execution_id"] = "";
    j["timestamp"] = "2024-01-15T14:30:00Z";

    auto tu = TradeUpdate::from_json(j);

    std::cout << "Response: event=" << trade_update_event_to_string(tu.event) << std::endl;
    EXPECT_EQ(tu.event, TradeUpdateEvent::NEW);
    std::cout << "PASSED" << std::endl;
}

TEST(TradeUpdateTest, ParseRejectedEvent) {
    std::cout << "\n=== Test: TradeUpdateTest::ParseRejectedEvent ===" << std::endl;
    std::cout << "Testing: WebSocket trade_updates - Parse rejected event" << std::endl;

    json j;
    j["event"] = "rejected";
    j["execution_id"] = "";
    j["timestamp"] = "2024-01-15T14:30:00Z";

    auto tu = TradeUpdate::from_json(j);

    std::cout << "Response: event=" << trade_update_event_to_string(tu.event) << std::endl;
    EXPECT_EQ(tu.event, TradeUpdateEvent::REJECTED);
    std::cout << "PASSED" << std::endl;
}

// ===== TradeUpdateEvent Enum Mapping Tests =====

TEST(TradeUpdateEventTest, AllEventToStringMappings) {
    std::cout << "\n=== Test: TradeUpdateEventTest::AllEventToStringMappings ===" << std::endl;
    std::cout << "Testing: TradeUpdateEvent enum -> string for all 18 events" << std::endl;

    struct { TradeUpdateEvent e; std::string expected; } cases[] = {
        {TradeUpdateEvent::NEW, "new"},
        {TradeUpdateEvent::FILL, "fill"},
        {TradeUpdateEvent::PARTIAL_FILL, "partial_fill"},
        {TradeUpdateEvent::CANCELED, "canceled"},
        {TradeUpdateEvent::EXPIRED, "expired"},
        {TradeUpdateEvent::DONE_FOR_DAY, "done_for_day"},
        {TradeUpdateEvent::REPLACED, "replaced"},
        {TradeUpdateEvent::ACCEPTED, "accepted"},
        {TradeUpdateEvent::REJECTED, "rejected"},
        {TradeUpdateEvent::PENDING_NEW, "pending_new"},
        {TradeUpdateEvent::STOPPED, "stopped"},
        {TradeUpdateEvent::PENDING_CANCEL, "pending_cancel"},
        {TradeUpdateEvent::PENDING_REPLACE, "pending_replace"},
        {TradeUpdateEvent::CALCULATED, "calculated"},
        {TradeUpdateEvent::SUSPENDED, "suspended"},
        {TradeUpdateEvent::ORDER_REPLACE_REJECTED, "order_replace_rejected"},
        {TradeUpdateEvent::ORDER_CANCEL_REJECTED, "order_cancel_rejected"},
        {TradeUpdateEvent::UNKNOWN, "unknown"},
    };

    for (const auto& tc : cases) {
        std::string result = trade_update_event_to_string(tc.e);
        std::cout << "  " << tc.expected << " -> " << result << std::endl;
        EXPECT_EQ(result, tc.expected);
    }

    std::cout << "PASSED" << std::endl;
}

TEST(TradeUpdateEventTest, AllStringToEventMappings) {
    std::cout << "\n=== Test: TradeUpdateEventTest::AllStringToEventMappings ===" << std::endl;
    std::cout << "Testing: string -> TradeUpdateEvent for all 18 events" << std::endl;

    struct { std::string str; TradeUpdateEvent expected; } cases[] = {
        {"new", TradeUpdateEvent::NEW},
        {"fill", TradeUpdateEvent::FILL},
        {"partial_fill", TradeUpdateEvent::PARTIAL_FILL},
        {"canceled", TradeUpdateEvent::CANCELED},
        {"expired", TradeUpdateEvent::EXPIRED},
        {"done_for_day", TradeUpdateEvent::DONE_FOR_DAY},
        {"replaced", TradeUpdateEvent::REPLACED},
        {"accepted", TradeUpdateEvent::ACCEPTED},
        {"rejected", TradeUpdateEvent::REJECTED},
        {"pending_new", TradeUpdateEvent::PENDING_NEW},
        {"stopped", TradeUpdateEvent::STOPPED},
        {"pending_cancel", TradeUpdateEvent::PENDING_CANCEL},
        {"pending_replace", TradeUpdateEvent::PENDING_REPLACE},
        {"calculated", TradeUpdateEvent::CALCULATED},
        {"suspended", TradeUpdateEvent::SUSPENDED},
        {"order_replace_rejected", TradeUpdateEvent::ORDER_REPLACE_REJECTED},
        {"order_cancel_rejected", TradeUpdateEvent::ORDER_CANCEL_REJECTED},
        {"some_unknown_event", TradeUpdateEvent::UNKNOWN},
    };

    for (const auto& tc : cases) {
        TradeUpdateEvent result = string_to_trade_update_event(tc.str);
        std::cout << "  \"" << tc.str << "\" -> " << trade_update_event_to_string(result) << std::endl;
        EXPECT_EQ(result, tc.expected);
    }

    std::cout << "PASSED" << std::endl;
}

TEST(TradeUpdateEventTest, RoundTripConversion) {
    std::cout << "\n=== Test: TradeUpdateEventTest::RoundTripConversion ===" << std::endl;
    std::cout << "Testing: TradeUpdateEvent enum -> string -> enum round-trip" << std::endl;

    std::vector<TradeUpdateEvent> events = {
        TradeUpdateEvent::NEW, TradeUpdateEvent::FILL, TradeUpdateEvent::PARTIAL_FILL,
        TradeUpdateEvent::CANCELED, TradeUpdateEvent::EXPIRED, TradeUpdateEvent::REPLACED,
        TradeUpdateEvent::ACCEPTED, TradeUpdateEvent::REJECTED, TradeUpdateEvent::PENDING_NEW,
        TradeUpdateEvent::PENDING_CANCEL, TradeUpdateEvent::PENDING_REPLACE,
    };

    for (auto e : events) {
        std::string s = trade_update_event_to_string(e);
        TradeUpdateEvent back = string_to_trade_update_event(s);
        std::cout << "  " << s << " -> round-trip OK" << std::endl;
        EXPECT_EQ(back, e);
    }

    std::cout << "PASSED" << std::endl;
}

// ===== Trade Updates Stream Protocol Tests =====

TEST(TradeUpdatesProtocolTest, ListenMessage) {
    std::cout << "\n=== Test: TradeUpdatesProtocolTest::ListenMessage ===" << std::endl;
    std::cout << "Testing: WebSocket trade_updates listen message" << std::endl;

    json listen;
    listen["action"] = "listen";
    listen["data"] = {{"streams", {"trade_updates"}}};

    std::cout << "Listen message: " << listen.dump(2) << std::endl;

    EXPECT_EQ(listen["action"], "listen");
    EXPECT_EQ(listen["data"]["streams"][0], "trade_updates");

    std::cout << "PASSED" << std::endl;
}

TEST(TradeUpdatesProtocolTest, TradeUpdateStreamMessage) {
    std::cout << "\n=== Test: TradeUpdatesProtocolTest::TradeUpdateStreamMessage ===" << std::endl;
    std::cout << "Testing: Parse incoming trade_updates stream message" << std::endl;

    json msg;
    msg["stream"] = "trade_updates";
    msg["data"]["event"] = "fill";
    msg["data"]["execution_id"] = "exec-123";
    msg["data"]["price"] = "185.50";
    msg["data"]["qty"] = "10";
    msg["data"]["position_qty"] = "10";
    msg["data"]["timestamp"] = "2024-01-15T14:30:00Z";

    std::cout << "Stream message: " << msg.dump(2) << std::endl;

    EXPECT_EQ(msg["stream"], "trade_updates");
    auto tu = TradeUpdate::from_json(msg["data"]);

    std::cout << "Parsed event: " << trade_update_event_to_string(tu.event) << std::endl;

    EXPECT_EQ(tu.event, TradeUpdateEvent::FILL);
    EXPECT_EQ(tu.price, Decimal(185.50));

    std::cout << "PASSED" << std::endl;
}

TEST(TradeUpdatesProtocolTest, AuthMessage) {
    std::cout << "\n=== Test: TradeUpdatesProtocolTest::AuthMessage ===" << std::endl;
    std::cout << "Testing: WebSocket trade_updates auth message" << std::endl;

    json auth;
    auth["action"] = "authenticate";
    auth["data"]["key_id"] = "APCA-API-KEY-ID";
    auth["data"]["secret_key"] = "APCA-API-SECRET-KEY";

    std::cout << "Auth message: " << auth.dump(2) << std::endl;

    EXPECT_EQ(auth["action"], "authenticate");
    EXPECT_TRUE(auth["data"].contains("key_id"));
    EXPECT_TRUE(auth["data"].contains("secret_key"));

    std::cout << "PASSED" << std::endl;
}
