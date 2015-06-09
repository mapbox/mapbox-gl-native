#include <mbgl/map/live_tile.hpp>
#include <mbgl/map/annotation.hpp>
#include <mbgl/util/math.hpp>
// #include <mbgl/util/constants.hpp>

namespace mbgl {

class LiveTileFeature : public GeometryTileFeature {
public:
    LiveTileFeature(const TileID&, const std::shared_ptr<const Annotation>&);

    FeatureType getType() const override;
    mapbox::util::optional<Value> getValue(const std::string&) const override;
    GeometryCollection getGeometries() const override;

private:
    const TileID id;
    const std::shared_ptr<const Annotation> annotation;
};


class LiveTileLayer : public GeometryTileLayer {
public:
    LiveTileLayer(const TileID& id_, std::vector<std::shared_ptr<const Annotation>>&& features_)
        : id(id_), features(std::move(features_)) {
    }

    std::size_t featureCount() const override {
        return features.size();
    }

    util::ptr<const GeometryTileFeature> getFeature(const std::size_t i) const override {
        if (features.size() > i) {
            return std::make_shared<LiveTileFeature>(id, features[i]);
        } else {
            return nullptr;
        }
    }

private:
    const TileID id;
    const std::vector<std::shared_ptr<const Annotation>> features;
};


LiveTileFeature::LiveTileFeature(const TileID& id_, const std::shared_ptr<const Annotation>& annotation_)
    : id(id_), annotation(annotation_) {}

FeatureType LiveTileFeature::getType() const {
    return annotation->getType() == AnnotationType::Point ? FeatureType::Point
                                                          : FeatureType::Unknown;
}

mapbox::util::optional<Value> LiveTileFeature::getValue(const std::string& key) const {
    // TODO: don't hardcode this.
    if (key == "sprite") {
        return { annotation->getSymbol() };
    } else {
        return {};
    }
}

GeometryCollection LiveTileFeature::getGeometries() const {
    constexpr int64_t extent = 4096;
    constexpr int64_t min = std::numeric_limits<int16_t>::min();
    constexpr int64_t max = std::numeric_limits<int16_t>::max();

    const auto& point = annotation->point;

    const uint64_t size = std::pow(2.0, id.z);
    const double sine = std::sin(point.latitude * M_PI / 180.0);
    const int64_t x = extent * ((point.longitude / 360.0 + 0.5) * size - id.x);
    const int64_t y = extent * ((0.5 - 0.25 * std::log((1.0 + sine) / (1.0 - sine)) / M_PI) * size - id.y);

    if (x >= min && x < max && y >= min && y < max) {
        return { { Coordinate{ static_cast<int16_t>(x), static_cast<int16_t>(y) } } };
    } else {
        return {};
    }
}

LiveTile::LiveTile(const AnnotationManager& manager_, const TileID& id_)
    : manager(manager_), id(id_) {
}

util::ptr<GeometryTileLayer> LiveTile::getLayer(const std::string& name) const {
    if (name == manager.layerID) {
        return std::make_shared<LiveTileLayer>(id, manager.getAnnotations(name, id));
    }
    return nullptr;
}

}
