#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile.hpp>

namespace mbgl {

class FillRenderable;

class FillBucket : public Bucket {
public:
    FillBucket();
    ~FillBucket() override;

    FillRenderable& getRenderable() const;

    void upload(gl::ObjectStore&) override;
    void render(Painter&, const style::Layer&, const UnwrappedTileID&, const mat4&) override;
    bool hasData() const override;
    bool needsClipping() const override;

    void addGeometry(const GeometryCollection&);

private:
    std::unique_ptr<FillRenderable> renderable;
};

} // namespace mbgl
