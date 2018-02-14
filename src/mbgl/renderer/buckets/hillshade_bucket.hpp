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

class HillshadeBucket : public Bucket {
public:
    HillshadeBucket(PremultipliedImage&&, Tileset::DEMEncoding encoding);
    HillshadeBucket(std::shared_ptr<PremultipliedImage>, Tileset::DEMEncoding encoding);
    HillshadeBucket(DEMData&&);


    void upload(gl::Context&) override;
    bool hasData() const override;

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
    gl::IndexVector<gl::Triangles> indices;
    SegmentVector<HillshadeAttributes> segments;

    optional<gl::VertexBuffer<HillshadeLayoutVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;
private: 
    DEMData demdata;
    bool prepared = false;
};

} // namespace mbgl
