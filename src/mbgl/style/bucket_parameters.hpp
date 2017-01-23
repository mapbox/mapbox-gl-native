#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/style/filter.hpp>

#include <atomic>
#include <functional>

namespace mbgl {

class GeometryTileLayer;
class GeometryTileFeature;
class FeatureIndex;

namespace style {

class BucketParameters {
public:
    const OverscaledTileID& tileID;
    const std::atomic<bool>& obsolete;
    FeatureIndex& featureIndex;
    const MapMode mode;

    bool cancelled() const {
        return obsolete;
    }

    void eachFilteredFeature(const Filter&,
                             const GeometryTileLayer&,
                             std::function<void (const GeometryTileFeature&, std::size_t index, const std::string& layerName)>);
};

} // namespace style
} // namespace mbgl
