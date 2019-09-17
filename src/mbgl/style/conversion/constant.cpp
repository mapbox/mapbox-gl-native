#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion_impl.hpp>

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

template <class T>
optional<T> Converter<T, typename std::enable_if_t<std::is_enum<T>::value>>::operator()(const Convertible& value, Error& error) const {
    optional<std::string> string = toString(value);
    if (!string) {
        error.message = "value must be a string";
        return nullopt;
    }

    const auto result = Enum<T>::toEnum(*string);
    if (!result) {
        error.message = "value must be a valid enumeration value";
        return nullopt;
    }

    return *result;
}

template <class T>
auto Converter<std::vector<T>, typename std::enable_if_t<std::is_enum<T>::value>>::operator()(const Convertible& value, Error& error) const -> optional<std::vector<T>> {
    if (!isArray(value)) {
        error.message = "value must be an array";
        return nullopt;
    }

    std::vector<T> result;
    result.reserve(arrayLength(value));

    for (std::size_t i = 0; i < arrayLength(value); ++i) {
        optional<T> enumItem = Converter<T>{}(arrayMember(value, i), error);
        if (!enumItem) {
            return nullopt;
        }
        result.push_back(*enumItem);
    }

    return result;
}

template optional<AlignmentType> Converter<AlignmentType>::operator()(const Convertible&, Error&) const;
template optional<CirclePitchScaleType> Converter<CirclePitchScaleType>::operator()(const Convertible&, Error&) const;
template optional<HillshadeIlluminationAnchorType> Converter<HillshadeIlluminationAnchorType>::operator()(const Convertible&, Error&) const;
template optional<IconTextFitType> Converter<IconTextFitType>::operator()(const Convertible&, Error&) const;
template optional<LightAnchorType> Converter<LightAnchorType>::operator()(const Convertible&, Error&) const;
template optional<LineCapType> Converter<LineCapType>::operator()(const Convertible&, Error&) const;
template optional<LineJoinType> Converter<LineJoinType>::operator()(const Convertible&, Error&) const;
template optional<RasterResamplingType> Converter<RasterResamplingType>::operator()(const Convertible&, Error&) const;
template optional<SymbolAnchorType> Converter<SymbolAnchorType>::operator()(const Convertible&, Error&) const;
template optional<SymbolPlacementType> Converter<SymbolPlacementType>::operator()(const Convertible&, Error&) const;
template optional<SymbolZOrderType> Converter<SymbolZOrderType>::operator()(const Convertible&, Error&) const;
template optional<TextJustifyType> Converter<TextJustifyType>::operator()(const Convertible&, Error&) const;
template optional<TextTransformType> Converter<TextTransformType>::operator()(const Convertible&, Error&) const;
template optional<TranslateAnchorType> Converter<TranslateAnchorType>::operator()(const Convertible&, Error&) const;
template optional<VisibilityType> Converter<VisibilityType>::operator()(const Convertible&, Error&) const;
template optional<std::vector<TextVariableAnchorType>> Converter<std::vector<TextVariableAnchorType>>::operator()(const Convertible&, Error&) const;
template optional<std::vector<TextWritingModeType>> Converter<std::vector<TextWritingModeType>>::operator()(const Convertible&, Error&) const;

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

template <size_t N>
optional<std::array<float, N>> Converter<std::array<float, N>>::operator()(const Convertible& value, Error& error) const {
    if (!isArray(value) || arrayLength(value) != N) {
        error.message = "value must be an array of " + util::toString(N) + " numbers";
        return nullopt;
    }

    std::array<float, N> result;
    for (size_t i = 0; i < N; i++) {
        optional<float> n = toNumber(arrayMember(value, i));
        if (!n) {
            error.message = "value must be an array of " + util::toString(N) + " numbers";
            return nullopt;
        }
        result[i] = *n;
    }
    return result;
}

template optional<std::array<float, 2>> Converter<std::array<float, 2>>::operator()(const Convertible&, Error&) const;
template optional<std::array<float, 3>> Converter<std::array<float, 3>>::operator()(const Convertible&, Error&) const;
template optional<std::array<float, 4>> Converter<std::array<float, 4>>::operator()(const Convertible&, Error&) const;

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
