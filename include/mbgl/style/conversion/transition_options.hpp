#pragma once

#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<TransitionOptions> {
public:
    optional<TransitionOptions> operator()(const Value& value, Error& error) const {
        if (!isObject(value)) {
            error = { "transition must be an object" };
            return {};
        }

        TransitionOptions result;

        auto duration = objectMember(value, "duration");
        if (duration) {
            auto number = toNumber(*duration);
            if (!number) {
                error = { "duration must be a number" };
                return {};
            }
            result.duration = { std::chrono::milliseconds(int64_t(*number)) };
        }

        auto delay = objectMember(value, "delay");
        if (delay) {
            auto number = toNumber(*delay);
            if (!number) {
                error = { "delay must be a number" };
                return {};
            }
            result.delay = { std::chrono::milliseconds(int64_t(*number)) };
        }

        return result;
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
