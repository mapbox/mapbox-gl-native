#include <string>
#include <mbgl/style/expression/check_subtype.hpp>

namespace mbgl {
namespace style {
namespace expression {
namespace type {

std::string errorMessage(const Type& expected, const Type& t) {
    return {"Expected " + toString(expected) + " but found " + toString(t) + " instead."};
}

optional<std::string> checkSubtype(const Type& expected, const Type& t) {
    if (t.is<ErrorType>()) return {};
    
    optional<std::string> result = expected.match(
        [&] (const Array& expectedArray) -> optional<std::string> {
            if (!t.is<Array>()) { return {errorMessage(expected, t)}; }
            const auto& actualArray = t.get<Array>();
            const auto err = checkSubtype(expectedArray.itemType, actualArray.itemType);
            if (err) return { errorMessage(expected, t) };
            if (expectedArray.N && expectedArray.N != actualArray.N) return { errorMessage(expected, t) };
            return {};
        },
        [&] (const ValueType&) -> optional<std::string> {
            if (t.is<ValueType>()) return {};
            
            const Type members[] = {
                Null,
                Boolean,
                Number,
                String,
                Object,
                Color,
                Array(Value)
            };
            
            for (const auto& member : members) {
                const auto err = checkSubtype(member, t);
                if (!err) {
                    return {};
                }
            }
            return { errorMessage(expected, t) };
        },
        [&] (const auto&) -> optional<std::string> {
            if (expected != t) {
                return { errorMessage(expected, t) };
            }
            return {};
        }
    );
    
    return result;
}

} // namespace type
} // namespace expression
} // namespace style
} // namespace mbgl
