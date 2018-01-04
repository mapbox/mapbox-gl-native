#include <mbgl/test/util.hpp>

#include <mbgl/style/function/interval_stops.hpp>

using namespace mbgl;
using namespace mbgl::style;

TEST(IntervalStops, Empty) {
    IntervalStops<float> stops;
    EXPECT_FALSE(bool(stops.evaluate(0)));
}

TEST(IntervalStops, NonNumericInput) {
    IntervalStops<float> stops(std::map<float, float> {{0.0f, 0.0f}});
    EXPECT_FALSE(bool(stops.evaluate(Value(NullValue()))));
    EXPECT_FALSE(bool(stops.evaluate(Value(false))));
    EXPECT_FALSE(bool(stops.evaluate(Value(std::string()))));
    EXPECT_FALSE(bool(stops.evaluate(Value(std::vector<Value>()))));
    EXPECT_FALSE(bool(stops.evaluate(Value(std::unordered_map<std::string, Value>()))));
}
