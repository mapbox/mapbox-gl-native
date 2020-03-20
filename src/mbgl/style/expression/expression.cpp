#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <utility>

namespace mbgl {
namespace style {
namespace expression {

class GeoJSONFeature : public GeometryTileFeature {
public:
    const Feature& feature;
    mutable optional<GeometryCollection> geometry;

    GeoJSONFeature(const Feature& feature_) : feature(feature_) {}
    GeoJSONFeature(const Feature& feature_, const CanonicalTileID& canonical) : feature(feature_) {
        geometry = convertGeometry(feature.geometry, canonical);
        // https://github.com/mapbox/geojson-vt-cpp/issues/44
        if (getType() == FeatureType::Polygon) {
            geometry = fixupPolygons(*geometry);
        }
    }

    FeatureType getType() const override  {
        return apply_visitor(ToFeatureType(), feature.geometry);
    }
    const PropertyMap& getProperties() const override { return feature.properties; }
    FeatureIdentifier getID() const override { return feature.id; }
    optional<mbgl::Value> getValue(const std::string& key) const override {
        auto it = feature.properties.find(key);
        if (it != feature.properties.end()) {
            return optional<mbgl::Value>(it->second);
        }
        return optional<mbgl::Value>();
    }
    const GeometryCollection& getGeometries() const override {
        if (geometry) return *geometry;
        geometry = GeometryCollection();
        return *geometry;
    }
};

EvaluationResult Expression::evaluate(optional<float> zoom,
                                      const Feature& feature,
                                      optional<double> colorRampParameter) const {
    GeoJSONFeature f(feature);
    return this->evaluate(EvaluationContext(std::move(zoom), &f, std::move(colorRampParameter)));
}

EvaluationResult Expression::evaluate(optional<float> zoom,
                                      const Feature& feature,
                                      optional<double> colorRampParameter,
                                      const std::set<std::string>& availableImages) const {
    GeoJSONFeature f(feature);
    return this->evaluate(
        EvaluationContext(std::move(zoom), &f, std::move(colorRampParameter)).withAvailableImages(&availableImages));
}

EvaluationResult Expression::evaluate(optional<float> zoom,
                                      const Feature& feature,
                                      optional<double> colorRampParameter,
                                      const std::set<std::string>& availableImages,
                                      const CanonicalTileID& canonical) const {
    GeoJSONFeature f(feature, canonical);
    return this->evaluate(EvaluationContext(std::move(zoom), &f, std::move(colorRampParameter))
                              .withAvailableImages(&availableImages)
                              .withCanonicalTileID(&canonical));
}

EvaluationResult Expression::evaluate(optional<mbgl::Value> accumulated, const Feature& feature) const {
    GeoJSONFeature f(feature);
    return this->evaluate(EvaluationContext(std::move(accumulated), &f));
}

} // namespace expression
} // namespace style
} // namespace mbgl
