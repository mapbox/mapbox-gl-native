#pragma once

#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/gfx/renderbuffer.hpp>
#include <mbgl/programs/background_program.hpp>
#include <mbgl/programs/heatmap_texture_program.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/raster_program.hpp>
#include <mbgl/util/optional.hpp>

#include <string>

namespace mbgl {
namespace gfx {
class Context;
class UploadPass;
} // namespace gfx

class RenderStaticData {
public:
    RenderStaticData(gfx::Context&, float pixelRatio);

    void upload(gfx::UploadPass&);

    optional<gfx::VertexBuffer<gfx::Vertex<PositionOnlyLayoutAttributes>>> tileVertexBuffer;
    optional<gfx::VertexBuffer<RasterLayoutVertex>> rasterVertexBuffer;
    optional<gfx::VertexBuffer<HeatmapTextureLayoutVertex>> heatmapTextureVertexBuffer;

    optional<gfx::IndexBuffer> quadTriangleIndexBuffer;
    optional<gfx::IndexBuffer> tileBorderIndexBuffer;

    SegmentVector<BackgroundAttributes> tileTriangleSegments;
    SegmentVector<DebugAttributes> tileBorderSegments;
    SegmentVector<RasterAttributes> rasterSegments;
    SegmentVector<HeatmapTextureAttributes> heatmapTextureSegments;

    optional<gfx::Renderbuffer<gfx::RenderbufferPixelType::Depth>> depthRenderbuffer;
    bool has3D = false;
    bool uploaded = false;
    Size backendSize;

    Programs programs;

#ifndef NDEBUG
    Programs overdrawPrograms;
#endif
};

} // namespace mbgl
