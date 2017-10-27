#pragma once

#include <mbgl/style/expression/type.hpp>
#include <mbgl/util/optional.hpp>
#include <memory>

namespace mbgl {
namespace style {
namespace expression {
namespace type {

optional<std::string> checkSubtype(const Type& expected, const Type& t);

} // namespace type
} // namespace expression
} // namespace style
} // namespace mbgl
