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
    optional<std::unique_ptr<Layer>> operator()(const Value& value, Error& error) const;
};

optional<Error> setPaintProperties(Layer& layer, const Value& value);

} // namespace conversion
} // namespace style
} // namespace mbgl
