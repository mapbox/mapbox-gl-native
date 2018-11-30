#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

#include <mbgl/util/feature.hpp>

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
    const PropertyMap& getProperties() const override { return feature.properties; }
    FeatureIdentifier getID() const override { return feature.id; }
    GeometryCollection getGeometries() const override { return {}; }
    mbgl::Value getValue(const std::string& key) const override {
        return feature.properties.count(key) ? feature.properties.at(key) : NullValue();
    }
};


EvaluationResult Expression::evaluate(optional<float> zoom, const Feature& feature, optional<double> colorRampParameter) const {
    GeoJSONFeature f(feature);
    return this->evaluate(EvaluationContext(zoom, &f, colorRampParameter));
}

} // namespace expression
} // namespace style
} // namespace mbgl
