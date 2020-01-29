#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/expression/collator_expression.hpp>


namespace mbgl {
namespace style {
namespace expression {

constexpr static const char filter[] = "filter-";

bool isFeatureConstant(const Expression& expression) {
    if (expression.getKind() == Kind::CompoundExpression) {
        auto e = static_cast<const CompoundExpression*>(&expression);
        const std::string name = e->getOperator();
        optional<std::size_t> parameterCount = e->getParameterCount();
        if (name == "get" && parameterCount && *parameterCount == 1) {
            return false;
        } else if (name == "has" && parameterCount && *parameterCount == 1) {
            return false;
        } else if (name == "properties" || name == "geometry-type" || name == "id" || name == "feature-state") {
            return false;
        } else if (0u == name.rfind(filter, 0u)) {
            // Legacy filters begin with "filter-" and are never constant.
            return false;
        }
    }

    if (expression.getKind() == Kind::FormatSectionOverride) {
        return false;
    }

    if (expression.getKind() == Kind::Within) {
        return false;
    }

    if (expression.getKind() == Kind::CollatorExpression) {
        // Although the results of a Collator expression with fixed arguments
        // generally shouldn't change between executions, we can't serialize them
        // as constant expressions because results change based on environment.
        return false;
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

bool isRuntimeConstant(const Expression& expression) {
    if (expression.getKind() == Kind::ImageExpression) {
        return false;
    }

    bool runtimeConstant = true;
    expression.eachChild([&](const Expression& e) {
        if (runtimeConstant && !isRuntimeConstant(e)) {
            runtimeConstant = false;
        }
    });
    return runtimeConstant;
}

} // namespace expression
} // namespace style
} // namespace mbgl
