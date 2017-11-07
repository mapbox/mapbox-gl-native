#include <mbgl/style/expression/is_constant.hpp>

namespace mbgl {
namespace style {
namespace expression {

bool isFeatureConstant(const Expression& expression) {
    if (auto e = dynamic_cast<const CompoundExpressionBase*>(&expression)) {
        const std::string name = e->getName();
        optional<std::size_t> parameterCount = e->getParameterCount();
        if (name == "get" && parameterCount && *parameterCount == 1) {
            return false;
        } else if (name == "has" && parameterCount && *parameterCount == 1) {
            return false;
        } else if (
            name == "properties" ||
            name == "geometry-type" ||
            name == "id"
        ) {
            return false;
        }
    }

    bool featureConstant = true;
    expression.eachChild([&](const Expression& e) {
        if (featureConstant && !isFeatureConstant(e)) {
            featureConstant = false;
        }
    });
    return featureConstant;
}

bool isZoomConstant(const Expression& e) {
    return isGlobalPropertyConstant(e, std::array<std::string, 1>{{"zoom"}});
}


} // namespace expression
} // namespace style
} // namespace mbgl
