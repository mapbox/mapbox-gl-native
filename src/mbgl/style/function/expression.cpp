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


EvaluationResult Expression::evaluate(optional<float> zoom, const Feature& feature, optional<double> rampEvaluationParameter) const {
    GeoJSONFeature f(feature);
    return this->evaluate(EvaluationContext(zoom, &f, rampEvaluationParameter));
}

} // namespace expression
} // namespace style
} // namespace mbgl
