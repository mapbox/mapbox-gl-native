#include <mbgl/style/conversion/constant.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<bool> Converter<bool>::operator()(const Convertible& value, Error& error) const {
    optional<bool> converted = toBool(value);
    if (!converted) {
        error = { "value must be a boolean" };
        return {};
    }
    return *converted;
}

optional<float> Converter<float>::operator()(const Convertible& value, Error& error) const {
    optional<float> converted = toNumber(value);
    if (!converted) {
        error = { "value must be a number" };
        return {};
    }
    return *converted;
}

optional<std::string> Converter<std::string>::operator()(const Convertible& value, Error& error) const {
    optional<std::string> converted = toString(value);
    if (!converted) {
        error = { "value must be a string" };
        return {};
    }
    return *converted;
}

optional<Color> Converter<Color>::operator()(const Convertible& value, Error& error) const {
    optional<std::string> string = toString(value);
    if (!string) {
        error = { "value must be a string" };
        return {};
    }

    optional<Color> color = Color::parse(*string);
    if (!color) {
        error = { "value must be a valid color" };
        return {};
    }

    return *color;
}

optional<std::vector<float>> Converter<std::vector<float>>::operator()(const Convertible& value, Error& error) const {
    if (!isArray(value)) {
        error = { "value must be an array" };
        return {};
    }

    std::vector<float> result;
    result.reserve(arrayLength(value));

    for (std::size_t i = 0; i < arrayLength(value); ++i) {
        optional<float> number = toNumber(arrayMember(value, i));
        if (!number) {
            error = { "value must be an array of numbers" };
            return {};
        }
        result.push_back(*number);
    }

    return result;
}

optional<std::vector<std::string>> Converter<std::vector<std::string>>::operator()(const Convertible& value, Error& error) const {
    if (!isArray(value)) {
        error = { "value must be an array" };
        return {};
    }

    std::vector<std::string> result;
    result.reserve(arrayLength(value));

    for (std::size_t i = 0; i < arrayLength(value); ++i) {
        optional<std::string> string = toString(arrayMember(value, i));
        if (!string) {
            error = { "value must be an array of strings" };
            return {};
        }
        result.push_back(*string);
    }

    return result;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
