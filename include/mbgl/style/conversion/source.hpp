#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/source.hpp>

#include <memory>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<std::unique_ptr<Source>> {
public:
    optional<std::unique_ptr<Source>> operator()(const Convertible& value, Error& error, const std::string& id) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
