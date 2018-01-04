#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/conversion.hpp>

#include <memory>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<std::unique_ptr<Layer>> {
public:
    optional<std::unique_ptr<Layer>> operator()(const Convertible& value, Error& error) const;
};

optional<Error> setLayoutProperty(Layer& layer, const std::string& name, const Convertible& value);
optional<Error> setPaintProperty(Layer& layer, const std::string& name, const Convertible& value);
optional<Error> setPaintProperties(Layer& layer, const Convertible& value);

} // namespace conversion
} // namespace style
} // namespace mbgl
