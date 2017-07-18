#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

#include <atomic>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

class RenderLayer;

class Bucket : private util::noncopyable {
public:
    Bucket() = default;
    virtual ~Bucket() = default;

    // Feature geometries are also used to populate the feature index.
    // Obtaining these is a costly operation, so we do it only once, and
    // pass-by-const-ref the geometries as a second parameter.
    virtual void addFeature(const GeometryTileFeature&,
                            const GeometryCollection&) {};

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

protected:
    std::atomic<bool> uploaded { false };
};

} // namespace mbgl
