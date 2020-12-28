#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/rotation.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<style::Rotation> {
    optional<style::Rotation> operator()(const Convertible& value, Error& error) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
