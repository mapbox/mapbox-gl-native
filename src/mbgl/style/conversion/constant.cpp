#include <mbgl/style/conversion/constant.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<bool> Converter<bool>::operator()(const Convertible& value, Error& error) const {
    optional<bool> converted = toBool(value);
    if (!converted) {
        error.message = "value must be a boolean";
        return nullopt;
    }
    return *converted;
}

optional<float> Converter<float>::operator()(const Convertible& value, Error& error) const {
    optional<float> converted = toNumber(value);
    if (!converted) {
        error.message = "value must be a number";
        return nullopt;
    }
    return *converted;
}

optional<std::string> Converter<std::string>::operator()(const Convertible& value, Error& error) const {
    optional<std::string> converted = toString(value);
    if (!converted) {
        error.message = "value must be a string";
        return nullopt;
    }
    return *converted;
}

optional<Color> Converter<Color>::operator()(const Convertible& value, Error& error) const {
    optional<std::string> string = toString(value);
    if (!string) {
        error.message = "value must be a string";
        return nullopt;
    }

    optional<Color> color = Color::parse(*string);
    if (!color) {
        error.message = "value must be a valid color";
        return nullopt;
    }

    return *color;
}

optional<std::vector<float>> Converter<std::vector<float>>::operator()(const Convertible& value, Error& error) const {
    if (!isArray(value)) {
        error.message = "value must be an array";
        return nullopt;
    }

    std::vector<float> result;
    result.reserve(arrayLength(value));

    for (std::size_t i = 0; i < arrayLength(value); ++i) {
        optional<float> number = toNumber(arrayMember(value, i));
        if (!number) {
            error.message = "value must be an array of numbers";
            return nullopt;
        }
        result.push_back(*number);
    }

    return result;
}

optional<std::vector<std::string>> Converter<std::vector<std::string>>::operator()(const Convertible& value, Error& error) const {
    if (!isArray(value)) {
        error.message = "value must be an array";
        return nullopt;
    }

    std::vector<std::string> result;
    result.reserve(arrayLength(value));

    for (std::size_t i = 0; i < arrayLength(value); ++i) {
        optional<std::string> string = toString(arrayMember(value, i));
        if (!string) {
            error.message = "value must be an array of strings";
            return nullopt;
        }
        result.push_back(*string);
    }

    return result;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
