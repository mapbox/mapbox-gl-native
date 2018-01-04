#pragma once

#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/util/optional.hpp>

#include <string>

namespace mbgl {

class RenderStaticData {
public:
    RenderStaticData(gl::Context&, float pixelRatio, const optional<std::string>& programCacheDir);

    gl::VertexBuffer<FillLayoutVertex> tileVertexBuffer;
    gl::VertexBuffer<RasterLayoutVertex> rasterVertexBuffer;
    gl::VertexBuffer<ExtrusionTextureLayoutVertex> extrusionTextureVertexBuffer;

    gl::IndexBuffer<gl::Triangles> quadTriangleIndexBuffer;
    gl::IndexBuffer<gl::LineStrip> tileBorderIndexBuffer;

    SegmentVector<FillAttributes> tileTriangleSegments;
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
