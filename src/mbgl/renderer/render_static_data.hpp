#pragma once

#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/gl/renderbuffer.hpp>
#include <mbgl/programs/background_program.hpp>
#include <mbgl/programs/extrusion_texture_program.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/raster_program.hpp>
#include <mbgl/util/optional.hpp>

#include <string>

namespace mbgl {
namespace gfx {
class Context;
} // namespace gfx

class RenderStaticData {
public:
    RenderStaticData(gfx::Context&, float pixelRatio, const optional<std::string>& programCacheDir);

    gfx::VertexBuffer<gfx::Vertex<PositionOnlyLayoutAttributes>> tileVertexBuffer;
    gfx::VertexBuffer<RasterLayoutVertex> rasterVertexBuffer;
    gfx::VertexBuffer<ExtrusionTextureLayoutVertex> extrusionTextureVertexBuffer;

    gfx::IndexBuffer quadTriangleIndexBuffer;
    gfx::IndexBuffer tileBorderIndexBuffer;

    SegmentVector<BackgroundAttributes> tileTriangleSegments;
    SegmentVector<DebugAttributes> tileBorderSegments;
    SegmentVector<RasterAttributes> rasterSegments;
    SegmentVector<ExtrusionTextureAttributes> extrusionTextureSegments;

    optional<gl::Renderbuffer<gl::RenderbufferType::DepthComponent>> depthRenderbuffer;
    bool has3D = false;
    Size backendSize;

    Programs programs;

#ifndef NDEBUG
    Programs overdrawPrograms;
#endif
};

} // namespace mbgl
