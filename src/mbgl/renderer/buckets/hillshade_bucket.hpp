#pragma once

#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/programs/hillshade_program.hpp>
#include <mbgl/programs/hillshade_prepare_program.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/tile_mask.hpp>
#include <mbgl/geometry/dem_data.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

class HillshadeBucket final : public Bucket {
public:
    HillshadeBucket(PremultipliedImage&&, Tileset::DEMEncoding encoding);
    HillshadeBucket(std::shared_ptr<PremultipliedImage>, Tileset::DEMEncoding encoding);
    HillshadeBucket(DEMData&&);
    ~HillshadeBucket() override;

    void upload(gl::Context&) override;
    bool hasData() const override;
    bool supportsLayer(const style::Layer::Impl&) const override;

    void clear();
    void setMask(TileMask&&);

    optional<gl::Texture> dem;
    optional<gl::Texture> texture;

    TileMask mask{ { 0, 0, 0 } };

    const DEMData& getDEMData() const;
    DEMData& getDEMData();

    bool isPrepared() const {
        return prepared;
    }

    void setPrepared (bool preparedState) {
        prepared = preparedState;
    }

    // Raster-DEM Tile Sources use the default buffers from Painter
    gl::VertexVector<HillshadeLayoutVertex> vertices;
    gl::IndexVector<gfx::Triangles> indices;
    SegmentVector<HillshadeAttributes> segments;

    optional<gl::VertexBuffer<HillshadeLayoutVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gfx::Triangles>> indexBuffer;
private: 
    DEMData demdata;
    bool prepared = false;
};

} // namespace mbgl
