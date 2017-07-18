#pragma once

#include <mbgl/map/transform_state.hpp>

#include <mbgl/tile/tile_id.hpp>

#include <mbgl/renderer/frame_history.hpp>
#include <mbgl/renderer/render_item.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/render_light.hpp>
#include <mbgl/renderer/render_pass.hpp>

#include <mbgl/gl/context.hpp>
#include <mbgl/programs/debug_program.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/programs/extrusion_texture_program.hpp>
#include <mbgl/programs/raster_program.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/offscreen_texture.hpp>

#include <mbgl/algorithm/generate_clip_ids.hpp>

#include <array>
#include <vector>

namespace mbgl {

class RenderStyle;
class ImageManager;
class View;
class LineAtlas;
class Programs;
class PaintParameters;
struct ClipID;

struct FrameData {
    TimePoint timePoint;
    float pixelRatio;
    MapMode mapMode;
    GLContextMode contextMode;
    MapDebugOptions debugOptions;
};

class Painter : private util::noncopyable {
public:
    Painter(gl::Context&, float pixelRatio, const optional<std::string>& programCacheDir);
    ~Painter();

    void render(RenderStyle&,
                const FrameData&,
                View&);

    void cleanup();
    bool needsAnimation() const;

    mat4 matrixForTile(const UnwrappedTileID&);
    gl::DepthMode depthModeForSublayer(uint8_t n, gl::DepthMode::Mask) const;
    gl::StencilMode stencilModeForClipping(const ClipID&) const;
    gl::ColorMode colorModeForRenderPass() const;

#ifndef NDEBUG
    PaintMode paintMode() const {
        return frame.debugOptions & MapDebugOptions::Overdraw ? PaintMode::Overdraw
                                                              : PaintMode::Regular;
    }
#else
    PaintMode paintMode() const {
        return PaintMode::Regular;
    }
#endif

    gl::Context& context;

    TransformState state;

    algorithm::ClipIDGenerator clipIDGenerator;

    mat4 projMatrix;
    mat4 nearClippedProjMatrix;

    std::array<float, 2> pixelsToGLUnits;

    const mat4 identityMatrix = []{
        mat4 identity;
        matrix::identity(identity);
        return identity;
    }();

    FrameData frame;

    int indent = 0;

    RenderPass pass = RenderPass::Opaque;

    int numSublayers = 3;
    uint32_t currentLayer;
    float depthRangeSize;
    const float depthEpsilon = 1.0f / (1 << 16);

    ImageManager* imageManager = nullptr;
    LineAtlas* lineAtlas = nullptr;

    optional<OffscreenTexture> extrusionTexture;

    EvaluatedLight evaluatedLight;

    FrameHistory frameHistory;

    std::unique_ptr<Programs> programs;
#ifndef NDEBUG
    std::unique_ptr<Programs> overdrawPrograms;
#endif

    gl::VertexBuffer<FillLayoutVertex> tileVertexBuffer;
    gl::VertexBuffer<RasterLayoutVertex> rasterVertexBuffer;
    gl::VertexBuffer<ExtrusionTextureLayoutVertex> extrusionTextureVertexBuffer;

    gl::IndexBuffer<gl::Triangles> quadTriangleIndexBuffer;
    gl::IndexBuffer<gl::LineStrip> tileBorderIndexBuffer;

    SegmentVector<FillAttributes> tileTriangleSegments;
    SegmentVector<DebugAttributes> tileBorderSegments;
    SegmentVector<RasterAttributes> rasterSegments;
    SegmentVector<ExtrusionTextureAttributes> extrusionTextureSegments;
};

} // namespace mbgl
