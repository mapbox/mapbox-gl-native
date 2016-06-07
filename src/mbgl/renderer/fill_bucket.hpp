#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile.hpp>

#include <vector>
#include <memory>

namespace mbgl {

class OutlinePatternShader;
class PlainShader;
class PatternShader;
class OutlineShader;
class FillRenderable;

class FillBucket : public Bucket {
public:
    FillBucket();
    ~FillBucket() override;

    void upload(gl::ObjectStore&) override;
    void render(Painter&, const style::Layer&, const UnwrappedTileID&, const mat4&) override;
    bool hasData() const override;
    bool needsClipping() const override;

    void addGeometry(const GeometryCollection&);

    void drawFill(PlainShader&, gl::ObjectStore&);
    void drawFill(PatternShader&, gl::ObjectStore&);
    void drawFillOutline(OutlineShader&, gl::ObjectStore&);
    void drawFillOutline(OutlinePatternShader&, gl::ObjectStore&);

private:
    std::unique_ptr<FillRenderable> renderable;
};

} // namespace mbgl
