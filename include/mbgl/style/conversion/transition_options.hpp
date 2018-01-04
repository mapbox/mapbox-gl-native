#pragma once

#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<TransitionOptions> {
public:
    optional<TransitionOptions> operator()(const Convertible& value, Error& error) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
