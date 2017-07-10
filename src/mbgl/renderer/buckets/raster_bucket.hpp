#pragma once

#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/programs/raster_program.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/indexed_primitives.hpp>
#include <mbgl/renderer/drawable.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

class RasterBucket : public Bucket {
public:
    RasterBucket(UnassociatedImage&&);

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const RenderLayer&, const RenderTile&) override;
    void render(Painter& painter,
                PaintParameters& parameters,
                const RenderLayer& layer,
                const mat4& matrix);
    bool hasData() const override;

    void clear();
    UnassociatedImage image;
    optional<gl::Texture> texture;


    // Bucket specific vertices are used for Image Sources only
    // Raster Tile Sources use the default buffers from Painter
    IndexedPrimitives<gl::Triangles, RasterLayoutVertex, RasterAttributes> primitives;
    optional<Drawable<gl::Triangles, RasterLayoutVertex, RasterAttributes>> drawable;
};

} // namespace mbgl
