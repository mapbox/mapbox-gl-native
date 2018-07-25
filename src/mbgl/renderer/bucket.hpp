#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/style/layer_type.hpp>

#include <atomic>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

class RenderLayer;

class Bucket : private util::noncopyable {
public:
    Bucket(style::LayerType layerType_)
        : layerType(layerType_) {
    }

    virtual ~Bucket() = default;

    // Check whether this bucket is of the given subtype.
    template <class T>
    bool is() const;

    // Dynamically cast this bucket to the given subtype.
    template <class T>
    T* as() {
        return is<T>() ? reinterpret_cast<T*>(this) : nullptr;
    }

    template <class T>
    const T* as() const {
        return is<T>() ? reinterpret_cast<const T*>(this) : nullptr;
    }

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
    style::LayerType layerType;
    std::atomic<bool> uploaded { false };
};

} // namespace mbgl
