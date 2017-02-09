#pragma once

#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<TransitionOptions> {
public:
    template <class V>
    Result<TransitionOptions> operator()(const V& value) const {
        if (!isObject(value)) {
            return Error { "transition must be an object" };
        }

        TransitionOptions result;

        auto duration = objectMember(value, "duration");
        if (duration) {
            auto number = toNumber(*duration);
            if (!number) {
                return Error { "duration must be a number" };
            }
            result.duration = { std::chrono::milliseconds(int64_t(*number)) };
        }

        auto delay = objectMember(value, "delay");
        if (delay) {
            auto number = toNumber(*delay);
            if (!number) {
                return Error { "delay must be a number" };
            }
            result.delay = { std::chrono::milliseconds(int64_t(*number)) };
        }

        return result;
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
