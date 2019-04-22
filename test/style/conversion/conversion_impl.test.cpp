#include <mbgl/test/util.hpp>
#include <mbgl/style/conversion_impl.hpp>

namespace mbgl {
namespace style {
namespace conversion {

class MockConvertible {
public:
    MockConvertible() = default;
    MockConvertible(int* counter_) : counter(counter_) {}

    MockConvertible(MockConvertible&& other) noexcept : counter(other.counter) {}

    ~MockConvertible() {
        ++*counter;
    }

    int* counter = nullptr;
};

template <>
class ConversionTraits<MockConvertible> {
public:
    static bool isUndefined(const MockConvertible&) {
        return false;
    }

    static bool isArray(const MockConvertible&) {
        return false;
    }

    static bool isObject(const MockConvertible&) {
        return false;
    }

    static std::size_t arrayLength(const MockConvertible&) {
        return 0u;
    }

    static MockConvertible arrayMember(const MockConvertible&, std::size_t) {
        return {};
    }

    static optional<MockConvertible> objectMember(const MockConvertible&, const char *) {
        return nullopt;
    }

    template <class Fn>
    static optional<Error> eachMember(const MockConvertible&, Fn&&) {
        return nullopt;
    }

    static optional<bool> toBool(const MockConvertible&) {
        return nullopt;
    }

    static optional<float> toNumber(const MockConvertible&) {
        return nullopt;
    }

    static optional<double> toDouble(const MockConvertible&) {
        return nullopt;
    }

    static optional<std::string> toString(const MockConvertible&) {
        return nullopt;
    }

    static optional<mbgl::Value> toValue(const MockConvertible&) {
        return nullopt;
    }

    static optional<GeoJSON> toGeoJSON(const MockConvertible&, Error&) {
        return nullopt;
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::style::conversion;

TEST(Conversion, Move) {
    int dtorCounter = 0;

    {
        MockConvertible mock(&dtorCounter);
        Convertible a(std::move(mock));
        Convertible b(std::move(a));
        a = std::move(b);
        Convertible* c = &a;
        a = std::move(*c);
    }

    ASSERT_EQ(dtorCounter, 4);
}
