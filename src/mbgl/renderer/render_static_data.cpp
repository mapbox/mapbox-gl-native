#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/programs/program_parameters.hpp>

namespace mbgl {

static gl::VertexVector<PositionOnlyLayoutAttributes::Vertex> tileVertices() {
    gl::VertexVector<PositionOnlyLayoutAttributes::Vertex> result;
    result.emplace_back(PositionOnlyLayoutAttributes::Vertex({{{ 0,            0 }}}));
    result.emplace_back(PositionOnlyLayoutAttributes::Vertex({{{ util::EXTENT, 0 }}}));
    result.emplace_back(PositionOnlyLayoutAttributes::Vertex({{{ 0, util::EXTENT }}}));
    result.emplace_back(PositionOnlyLayoutAttributes::Vertex({{{ util::EXTENT, util::EXTENT }}}));
    return result;
}

static gl::IndexVector<gl::Triangles> quadTriangleIndices() {
    gl::IndexVector<gl::Triangles> result;
    result.emplace_back(0, 1, 2);
    result.emplace_back(1, 2, 3);
    return result;
}

static gl::IndexVector<gl::LineStrip> tileLineStripIndices() {
    gl::IndexVector<gl::LineStrip> result;
    result.emplace_back(0);
    result.emplace_back(1);
    result.emplace_back(3);
    result.emplace_back(2);
    result.emplace_back(0);
    return result;
}

static gl::VertexVector<RasterLayoutVertex> rasterVertices() {
    gl::VertexVector<RasterLayoutVertex> result;
    result.emplace_back(RasterProgram::layoutVertex({ 0, 0 }, { 0, 0 }));
    result.emplace_back(RasterProgram::layoutVertex({ util::EXTENT, 0 }, { util::EXTENT, 0 }));
    result.emplace_back(RasterProgram::layoutVertex({ 0, util::EXTENT }, { 0, util::EXTENT }));
    result.emplace_back(RasterProgram::layoutVertex({ util::EXTENT, util::EXTENT }, { util::EXTENT, util::EXTENT }));
    return result;
}

static gl::VertexVector<ExtrusionTextureLayoutVertex> extrusionTextureVertices() {
    gl::VertexVector<ExtrusionTextureLayoutVertex> result;
    result.emplace_back(ExtrusionTextureProgram::layoutVertex({ 0, 0 }));
    result.emplace_back(ExtrusionTextureProgram::layoutVertex({ 1, 0 }));
    result.emplace_back(ExtrusionTextureProgram::layoutVertex({ 0, 1 }));
    result.emplace_back(ExtrusionTextureProgram::layoutVertex({ 1, 1 }));
    return result;
}

RenderStaticData::RenderStaticData(gl::Context& context, float pixelRatio, const optional<std::string>& programCacheDir)
    : tileVertexBuffer(context.createVertexBuffer(tileVertices())),
      rasterVertexBuffer(context.createVertexBuffer(rasterVertices())),
      extrusionTextureVertexBuffer(context.createVertexBuffer(extrusionTextureVertices())),
      quadTriangleIndexBuffer(context.createIndexBuffer(quadTriangleIndices())),
      tileBorderIndexBuffer(context.createIndexBuffer(tileLineStripIndices())),
      programs(context, ProgramParameters { pixelRatio, false, programCacheDir })
#ifndef NDEBUG
    , overdrawPrograms(context, ProgramParameters { pixelRatio, true, programCacheDir })
#endif
{
    tileTriangleSegments.emplace_back(0, 0, 4, 6);
    tileBorderSegments.emplace_back(0, 0, 4, 5);
    rasterSegments.emplace_back(0, 0, 4, 6);
    extrusionTextureSegments.emplace_back(0, 0, 4, 6);
}

} // namespace mbgl
