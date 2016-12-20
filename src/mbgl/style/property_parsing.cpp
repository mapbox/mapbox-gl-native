#include <mbgl/style/property_parsing.hpp>

namespace mbgl {
namespace style {

optional<TransitionOptions> parseTransitionOptions(const char *, const JSValue& value) {
    if (!value.IsObject()) {
        return {};
    }

    optional<Duration> duration;
    if (value.HasMember("duration") && value["duration"].IsNumber()) {
        duration.emplace(Milliseconds(value["duration"].GetUint()));
    }

    optional<Duration> delay;
    if (value.HasMember("delay") && value["delay"].IsNumber()) {
        delay.emplace(Milliseconds(value["delay"].GetUint()));
    }

    if (!duration && !delay) {
        return {};
    }

    TransitionOptions options;
    options.duration = std::move(duration);
    options.delay = std::move(delay);

    return options;
}

} // namespace style
} // namespace mbgl
