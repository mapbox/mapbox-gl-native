#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/map/mode.hpp>

namespace mbgl {

class CircleRenderable;
class CircleShader;

class CircleBucket : public Bucket {
public:
    CircleBucket(const MapMode);
    ~CircleBucket() override;

    void upload(gl::ObjectStore&) override;
    void render(Painter&, const style::Layer&, const UnwrappedTileID&, const mat4&) override;

    bool hasData() const override;
    bool needsClipping() const override;
    void addGeometry(const GeometryCollection&);

    void drawCircles(CircleShader&, gl::ObjectStore&);

private:
    const MapMode mode;

    std::unique_ptr<CircleRenderable> renderable;
};

} // namespace mbgl
