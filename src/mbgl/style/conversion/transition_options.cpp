#include <mbgl/style/conversion/transition_options.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<TransitionOptions> Converter<TransitionOptions>::operator()(const Convertible& value, Error& error) const {
    if (!isObject(value)) {
        error = { "transition must be an object" };
        return {};
    }

    auto objectDuration = objectMember(value, "duration");
    optional<Duration> duration;
    if (objectDuration) {
        auto number = toNumber(*objectDuration);
        if (!number) {
            error = { "duration must be a number" };
            return {};
        }
        duration = Duration(std::chrono::milliseconds(int64_t(*number)));
    }

    auto objectDelay = objectMember(value, "delay");
    optional<Duration> delay;
    if (objectDelay) {
        auto number = toNumber(*objectDelay);
        if (!number) {
            error = { "delay must be a number" };
            return {};
        }
        delay = Duration(std::chrono::milliseconds(int64_t(*number)));
    }

    return TransitionOptions{duration, delay};
}

} // namespace conversion
} // namespace style
} // namespace mbgl
