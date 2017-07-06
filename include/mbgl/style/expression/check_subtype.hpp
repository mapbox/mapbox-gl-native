#pragma once

#include <memory>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {
namespace expression {
namespace type {

optional<std::string> checkSubtype(const Type& expected, const Type& t);

} // namespace type
} // namespace expression
} // namespace style
} // namespace mbgl
