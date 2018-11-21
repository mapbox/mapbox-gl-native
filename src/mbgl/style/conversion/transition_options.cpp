#include <mbgl/style/conversion/transition_options.hpp>
#include <mbgl/style/conversion_impl.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<TransitionOptions> Converter<TransitionOptions>::operator()(const Convertible& value, Error& error) const {
    if (!isObject(value)) {
        error.message = "transition must be an object";
        return nullopt;
    }

    optional<TransitionOptions> result = TransitionOptions{};

    auto duration = objectMember(value, "duration");
    if (duration) {
        auto number = toNumber(*duration);
        if (!number) {
            error.message = "duration must be a number";
            return nullopt;
        }
        result->duration = { std::chrono::milliseconds(int64_t(*number)) };
    }

    auto delay = objectMember(value, "delay");
    if (delay) {
        auto number = toNumber(*delay);
        if (!number) {
            error.message = "delay must be a number";
            return nullopt;
        }
        result->delay = { std::chrono::milliseconds(int64_t(*number)) };
    }

    return result;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
