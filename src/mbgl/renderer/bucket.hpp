#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <atomic>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

class RenderLayer;
class PatternDependency;
using PatternLayerMap = std::map<std::string, PatternDependency>;

class Bucket {
public:
    Bucket(const Bucket&) = delete;
    Bucket& operator=(const Bucket&) = delete;

    virtual ~Bucket() = default;

    // Feature geometries are also used to populate the feature index.
    // Obtaining these is a costly operation, so we do it only once, and
    // pass-by-const-ref the geometries as a second parameter.
    virtual void addFeature(const GeometryTileFeature&,
                            const GeometryCollection&,
                            const ImagePositions&,
                            const PatternLayerMap&) {};

    virtual void populateFeatureBuffers(const ImagePositions&) {};
    virtual void addPatternDependencies(const std::vector<const RenderLayer*>&, ImageDependencies&) {};

    // As long as this bucket has a Prepare render pass, this function is getting called. Typically,
    // this only happens once when the bucket is being rendered for the first time.
    virtual void upload(gl::Context&) = 0;

    virtual bool hasData() const = 0;

    virtual float getQueryRadius(const RenderLayer&) const {
        return 0;
    };

    bool needsUpload() const {
        return hasData() && !uploaded;
    }
    // Returns true if this bucket fits the given layer; returns false otherwise.
    // Implementations of this class check at least that this bucket has
    // the same layer type with the given layer, but extra checks are also
    // possible.
    virtual bool supportsLayer(const style::Layer::Impl&) const = 0;

protected:
    Bucket() = default;
    std::atomic<bool> uploaded { false };
};

} // namespace mbgl
