#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {
namespace style {
namespace expression {

class GeoJSONFeature : public GeometryTileFeature {
public:
    const Feature& feature;

    GeoJSONFeature(const Feature& feature_) : feature(feature_) {}

    FeatureType getType() const override  {
        return apply_visitor(ToFeatureType(), feature.geometry);
    }
    PropertyMap getProperties() const override { return feature.properties; }
    optional<FeatureIdentifier> getID() const override { return feature.id; }
    GeometryCollection getGeometries() const override { return {}; }
    optional<mbgl::Value> getValue(const std::string& key) const override {
        auto it = feature.properties.find(key);
        if (it != feature.properties.end()) {
            return optional<mbgl::Value>(it->second);
        }
        return optional<mbgl::Value>();
    }
};

bool Expression::isFeatureConstant() const {
    bool featureConstant = true;
    accept([&](const Expression* expression) {
        if (auto e = dynamic_cast<const CompoundExpressionBase*>(expression)) {
            const std::string name = e->getName();
            if (name == "get" || name == "has") {
                optional<std::size_t> parameterCount = e->getParameterCount();
                featureConstant = featureConstant && (parameterCount && *parameterCount > 1);
            } else {
                featureConstant = featureConstant && !(
                    name == "properties" ||
                    name == "geometry-type" ||
                    name == "id"
                );
            }
        }
    });
    return featureConstant;
}

bool Expression::isZoomConstant() const {
    bool zoomConstant = true;
    accept([&](const Expression* expression) {
        if (auto e = dynamic_cast<const CompoundExpressionBase*>(expression)) {
            if (e->getName() == "zoom") {
                zoomConstant = false;
            }
        }
    });
    return zoomConstant;
}

EvaluationResult Expression::evaluate(float z, const Feature& feature) const {
    GeoJSONFeature f(feature);
    return this->evaluate(EvaluationParameters(z, &f));
}

} // namespace expression
} // namespace style
} // namespace mbgl
