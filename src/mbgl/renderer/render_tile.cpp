#include <mbgl/renderer/render_tile.hpp>

#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/buckets/debug_bucket.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/renderer/tile_render_data.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

using namespace style;

RenderTile::RenderTile(UnwrappedTileID id_, Tile& tile_) 
    : id(std::move(id_)), tile(tile_) {
}

RenderTile::~RenderTile() = default;

mat4 RenderTile::translateVtxMatrix(const mat4& tileMatrix,
                                    const std::array<float, 2>& translation,
                                    TranslateAnchorType anchor,
                                    const TransformState& state,
                                    const bool inViewportPixelUnits) const {
    if (translation[0] == 0 && translation[1] == 0) {
        return tileMatrix;
    }

    mat4 vtxMatrix;

    const float angle = inViewportPixelUnits ?
        (anchor == TranslateAnchorType::Map ? state.getBearing() : 0) :
        (anchor == TranslateAnchorType::Viewport ? -state.getBearing() : 0);

    Point<float> translate = util::rotate(Point<float>{ translation[0], translation[1] }, angle);

    if (inViewportPixelUnits) {
        matrix::translate(vtxMatrix, tileMatrix, translate.x, translate.y, 0);
    } else {
        matrix::translate(vtxMatrix, tileMatrix,
                          id.pixelsToTileUnits(translate.x, state.getZoom()),
                          id.pixelsToTileUnits(translate.y, state.getZoom()),
                          0);
    }

    return vtxMatrix;
}

mat4 RenderTile::translatedMatrix(const std::array<float, 2>& translation,
                                  TranslateAnchorType anchor,
                                  const TransformState& state) const {
    return translateVtxMatrix(matrix, translation, anchor, state, false);
}

mat4 RenderTile::translatedClipMatrix(const std::array<float, 2>& translation,
                                      TranslateAnchorType anchor,
                                      const TransformState& state) const {
    return translateVtxMatrix(nearClippedMatrix, translation, anchor, state, false);
}

const OverscaledTileID& RenderTile::getOverscaledTileID() const { return tile.id; }
bool RenderTile::holdForFade() const { return tile.holdForFade(); }

Bucket* RenderTile::getBucket(const style::Layer::Impl& impl) const {
    assert(renderData);
    return renderData->getBucket(impl);
}

const LayerRenderData* RenderTile::getLayerRenderData(const style::Layer::Impl& impl) const {
    assert(renderData);
    return renderData->getLayerRenderData(impl);
}

optional<ImagePosition> RenderTile::getPattern(const std::string& pattern) const {
    assert(renderData);
    return renderData->getPattern(pattern);
}

const gfx::Texture& RenderTile::getGlyphAtlasTexture() const {
    assert(renderData);
    return renderData->getGlyphAtlasTexture();
}

const gfx::Texture& RenderTile::getIconAtlasTexture() const {
    assert(renderData);
    return renderData->getIconAtlasTexture();
}

void RenderTile::upload(gfx::UploadPass& uploadPass) const {
    assert(renderData);
    renderData->upload(uploadPass);

    if (debugBucket) {
        debugBucket->upload(uploadPass);
    }
}

void RenderTile::prepare(const SourcePrepareParameters& parameters) {
    renderData = tile.createRenderData();
    assert(renderData);
    renderData->prepare(parameters);

    needsRendering = tile.usedByRenderedLayers;

    if (parameters.debugOptions != MapDebugOptions::NoDebug &&
        (!debugBucket || debugBucket->renderable != tile.isRenderable() ||
         debugBucket->complete != tile.isComplete() ||
         !(debugBucket->modified == tile.modified) ||
         !(debugBucket->expires == tile.expires) ||
         debugBucket->debugMode != parameters.debugOptions)) {
        debugBucket = std::make_unique<DebugBucket>(
            tile.id, tile.isRenderable(), tile.isComplete(), tile.modified, tile.expires,
            parameters.debugOptions);
    } else if (parameters.debugOptions == MapDebugOptions::NoDebug) {
        debugBucket.reset();
    }

    // Calculate two matrices for this tile: matrix is the standard tile matrix; nearClippedMatrix
    // has near plane moved further, to enhance depth buffer precision
    const auto& transform = parameters.transform;
    transform.state.matrixFor(matrix, id);
    transform.state.matrixFor(nearClippedMatrix, id);
    matrix::multiply(matrix, transform.projMatrix, matrix);
    matrix::multiply(nearClippedMatrix, transform.nearClippedProjMatrix, nearClippedMatrix);
}

void RenderTile::finishRender(PaintParameters& parameters) const {
    if (!needsRendering || parameters.debugOptions == MapDebugOptions::NoDebug)
        return;

    static const style::Properties<>::PossiblyEvaluated properties {};
    static const DebugProgram::Binders paintAttributeData(properties, 0);

    auto& program = parameters.programs.debug;

    if (parameters.debugOptions & (MapDebugOptions::Timestamps | MapDebugOptions::ParseStatus)) {
        assert(debugBucket);
        const auto allAttributeBindings = program.computeAllAttributeBindings(
            *debugBucket->vertexBuffer,
            paintAttributeData,
            properties
        );

        program.draw(
            parameters.context,
            *parameters.renderPass,
            gfx::Lines { 4.0f * parameters.pixelRatio },
            gfx::DepthMode::disabled(),
            gfx::StencilMode::disabled(),
            gfx::ColorMode::unblended(),
            gfx::CullFaceMode::disabled(),
            *debugBucket->indexBuffer,
            debugBucket->segments,
            program.computeAllUniformValues(
                DebugProgram::LayoutUniformValues {
                    uniforms::matrix::Value( matrix ),
                    uniforms::color::Value( Color::white() )
                },
                paintAttributeData,
                properties,
                parameters.state.getZoom()
            ),
            allAttributeBindings,
            DebugProgram::TextureBindings{},
            "__debug/" + debugBucket->drawScopeID + "/text-outline"
        );

        program.draw(
            parameters.context,
            *parameters.renderPass,
            gfx::Lines { 2.0f * parameters.pixelRatio },
            gfx::DepthMode::disabled(),
            gfx::StencilMode::disabled(),
            gfx::ColorMode::unblended(),
            gfx::CullFaceMode::disabled(),
            *debugBucket->indexBuffer,
            debugBucket->segments,
            program.computeAllUniformValues(
                DebugProgram::LayoutUniformValues {
                    uniforms::matrix::Value( matrix ),
                    uniforms::color::Value( Color::black() )
                },
                paintAttributeData,
                properties,
                parameters.state.getZoom()
            ),
            allAttributeBindings,
            DebugProgram::TextureBindings{},
            "__debug/" + debugBucket->drawScopeID + "/text"
        );
    }

    if (parameters.debugOptions & MapDebugOptions::TileBorders) {
        assert(debugBucket);
        parameters.programs.debug.draw(
            parameters.context,
            *parameters.renderPass,
            gfx::LineStrip { 4.0f * parameters.pixelRatio },
            gfx::DepthMode::disabled(),
            gfx::StencilMode::disabled(),
            gfx::ColorMode::unblended(),
            gfx::CullFaceMode::disabled(),
            *parameters.staticData.tileBorderIndexBuffer,
            parameters.staticData.tileBorderSegments,
            program.computeAllUniformValues(
                DebugProgram::LayoutUniformValues {
                    uniforms::matrix::Value( matrix ),
                    uniforms::color::Value( Color::red() )
                },
                paintAttributeData,
                properties,
                parameters.state.getZoom()
            ),
            program.computeAllAttributeBindings(
                *parameters.staticData.tileVertexBuffer,
                paintAttributeData,
                properties
            ),
            DebugProgram::TextureBindings{},
            "__debug/" + debugBucket->drawScopeID
        );
    }
}

} // namespace mbgl
