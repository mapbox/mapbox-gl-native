#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_style.hpp>
#include <mbgl/renderer/indexed_primitives.hpp>

#include <mbgl/style/source.hpp>
#include <mbgl/style/source_impl.hpp>

#include <mbgl/map/view.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/gl/debugging.hpp>

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>

#include <mbgl/tile/tile.hpp>
#include <mbgl/renderer/layers/render_background_layer.hpp>
#include <mbgl/renderer/layers/render_custom_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>

#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/geometry/line_atlas.hpp>

#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/programs/programs.hpp>

#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat3.hpp>
#include <mbgl/util/string.hpp>

#include <mbgl/util/stopwatch.hpp>

#include <cassert>
#include <algorithm>
#include <iostream>
#include <unordered_set>

namespace mbgl {

using namespace style;

static auto rasterPrimitives() {
    IndexedPrimitives<gl::Triangles, RasterLayoutVertex, RasterAttributes> primitives;
    primitives.add(
        {
            RasterProgram::layoutVertex({ 0,            0            }, { 0,     0 }),
            RasterProgram::layoutVertex({ util::EXTENT, 0            }, { 32767, 0 }),
            RasterProgram::layoutVertex({ 0,            util::EXTENT }, { 0,     32767 }),
            RasterProgram::layoutVertex({ util::EXTENT, util::EXTENT }, { 32767, 32767 }),
        }, {
            {{ 0, 1, 2 }},
            {{ 1, 2, 3 }},
        }
    );
    return primitives;
}

static auto borderPrimitives() {
    IndexedPrimitives<gl::LineStrip, DebugLayoutVertex, DebugAttributes> primitives;
    primitives.add(
        {
            DebugProgram::layoutVertex({ 0,            0 }),
            DebugProgram::layoutVertex({ util::EXTENT, 0 }),
            DebugProgram::layoutVertex({ util::EXTENT, util::EXTENT }),
            DebugProgram::layoutVertex({ 0,            util::EXTENT }),
        }, {
            {{ 0 }},
            {{ 1 }},
            {{ 2 }},
            {{ 3 }},
            {{ 0 }},
        }
    );
    return primitives;
}

static auto fillPrimitives() {
    IndexedPrimitives<gl::Triangles, FillLayoutVertex, FillAttributes> primitives;
    primitives.add(
        {
            FillProgram::layoutVertex({ 0,            0 }),
            FillProgram::layoutVertex({ util::EXTENT, 0 }),
            FillProgram::layoutVertex({ 0,            util::EXTENT }),
            FillProgram::layoutVertex({ util::EXTENT, util::EXTENT }),
        }, {
            {{ 0, 1, 2 }},
            {{ 1, 2, 3 }},
        }
    );
    return primitives;
}

static auto extrusionTexturePrimitives() {
    IndexedPrimitives<gl::Triangles, ExtrusionTextureLayoutVertex, ExtrusionTextureAttributes> primitives;
    primitives.add(
        {
            ExtrusionTextureProgram::layoutVertex({ 0, 0 }),
            ExtrusionTextureProgram::layoutVertex({ 1, 0 }),
            ExtrusionTextureProgram::layoutVertex({ 0, 1 }),
            ExtrusionTextureProgram::layoutVertex({ 1, 1 }),
        }, {
            {{ 0, 1, 2 }},
            {{ 1, 2, 3 }},
        }
    );
    return primitives;
}

Painter::Painter(gl::Context& context_,
                 const TransformState& state_,
                 float pixelRatio,
                 const optional<std::string>& programCacheDir)
    : context(context_),
      state(state_),
      rasterDrawable(context, rasterPrimitives()),
      fillDrawable(context, fillPrimitives()),
      borderDrawable(context, borderPrimitives()),
      extrusionTextureDrawable(context, extrusionTexturePrimitives()) {
    programs = std::make_unique<Programs>(context,
                                          ProgramParameters{ pixelRatio, false, programCacheDir });
#ifndef NDEBUG
    overdrawPrograms =
        std::make_unique<Programs>(context, ProgramParameters{ pixelRatio, true, programCacheDir });
#endif
}

Painter::~Painter() = default;

bool Painter::needsAnimation() const {
    return frameHistory.needsAnimation(util::DEFAULT_TRANSITION_DURATION);
}

void Painter::cleanup() {
    context.performCleanup();
}

void Painter::render(RenderStyle& style, const FrameData& frame_, View& view) {
    frame = frame_;
    if (frame.contextMode == GLContextMode::Shared) {
        context.setDirtyState();
    }

    PaintParameters parameters {
#ifndef NDEBUG
        paintMode() == PaintMode::Overdraw ? *overdrawPrograms : *programs,
#else
        *programs,
#endif
        view
    };

    imageManager = style.imageManager.get();
    lineAtlas = style.lineAtlas.get();

    evaluatedLight = style.getRenderLight().getEvaluated();

    RenderData renderData = style.getRenderData(frame.debugOptions, state.getAngle());
    const std::vector<RenderItem>& order = renderData.order;
    const std::unordered_set<RenderSource*>& sources = renderData.sources;

    // Update the default matrices to the current viewport dimensions.
    state.getProjMatrix(projMatrix);
    // Calculate a second projection matrix with the near plane clipped to 100 so as
    // not to waste lots of depth buffer precision on very close empty space, for layer
    // types (fill-extrusion) that use the depth buffer to emulate real-world space.
    state.getProjMatrix(nearClippedProjMatrix, 100);

    pixelsToGLUnits = {{ 2.0f  / state.getSize().width, -2.0f / state.getSize().height }};
    if (state.getViewportMode() == ViewportMode::FlippedY) {
        pixelsToGLUnits[1] *= -1;
    }

    frameHistory.record(frame.timePoint, state.getZoom(),
        frame.mapMode == MapMode::Continuous ? util::DEFAULT_TRANSITION_DURATION : Milliseconds(0));


    // - UPLOAD PASS -------------------------------------------------------------------------------
    // Uploads all required buffers and images before we do any actual rendering.
    {
        MBGL_DEBUG_GROUP(context, "upload");

        imageManager->upload(context, 0);
        lineAtlas->upload(context, 0);
        frameHistory.upload(context, 0);
    }

    // - CLEAR -------------------------------------------------------------------------------------
    // Renders the backdrop of the OpenGL view. This also paints in areas where we don't have any
    // tiles whatsoever.
    {
        MBGL_DEBUG_GROUP(context, "clear");
        view.bind();
        context.clear(paintMode() == PaintMode::Overdraw
                        ? Color::black()
                        : renderData.backgroundColor,
                      1.0f,
                      0);
    }

    // - CLIPPING MASKS ----------------------------------------------------------------------------
    // Draws the clipping masks to the stencil buffer.
    {
        MBGL_DEBUG_GROUP(context, "clip");

        // Update all clipping IDs.
        clipIDGenerator = algorithm::ClipIDGenerator();
        for (const auto& source : sources) {
            source->startRender(*this);
        }

        MBGL_DEBUG_GROUP(context, "clipping masks");

        for (const auto& clipID : clipIDGenerator.getClipIDs()) {
            MBGL_DEBUG_GROUP(context, std::string{ "mask: " } + util::toString(clipID.first));
            renderClippingMask(clipID.first, clipID.second);
        }
    }

#if not MBGL_USE_GLES2 and not defined(NDEBUG)
    if (frame.debugOptions & MapDebugOptions::StencilClip) {
        renderClipMasks(parameters);
        return;
    }
#endif

    // Actually render the layers
    if (debug::renderTree) { Log::Info(Event::Render, "{"); indent++; }

    depthRangeSize = 1 - (order.size() + 2) * numSublayers * depthEpsilon;

    // - OPAQUE PASS -------------------------------------------------------------------------------
    // Render everything top-to-bottom by using reverse iterators. Render opaque objects first.
    renderPass(parameters,
               RenderPass::Opaque,
               order.rbegin(), order.rend(),
               0, 1);

    // - TRANSLUCENT PASS --------------------------------------------------------------------------
    // Make a second pass, rendering translucent objects. This time, we render bottom-to-top.
    renderPass(parameters,
               RenderPass::Translucent,
               order.begin(), order.end(),
               static_cast<uint32_t>(order.size()) - 1, -1);

    if (debug::renderTree) { Log::Info(Event::Render, "}"); indent--; }

    // - DEBUG PASS --------------------------------------------------------------------------------
    // Renders debug overlays.
    {
        MBGL_DEBUG_GROUP(context, "debug");

        // Finalize the rendering, e.g. by calling debug render calls per tile.
        // This guarantees that we have at least one function per tile called.
        // When only rendering layers via the stylesheet, it's possible that we don't
        // ever visit a tile during rendering.
        for (const auto& source : sources) {
            source->finishRender(*this);
        }
    }

#if not MBGL_USE_GLES2 and not defined(NDEBUG)
    if (frame.debugOptions & MapDebugOptions::DepthBuffer) {
        renderDepthBuffer(parameters);
    }
#endif

    // TODO: Find a better way to unbind VAOs after we're done with them without introducing
    // unnecessary bind(0)/bind(N) sequences.
    {
        MBGL_DEBUG_GROUP(context, "cleanup");

        context.activeTexture = 1;
        context.texture[1] = 0;
        context.activeTexture = 0;
        context.texture[0] = 0;

        context.vertexArrayObject = 0;
    }
}

template <class Iterator>
void Painter::renderPass(PaintParameters& parameters,
                         RenderPass pass_,
                         Iterator it, Iterator end,
                         uint32_t i, int8_t increment) {
    pass = pass_;

    MBGL_DEBUG_GROUP(context, pass == RenderPass::Opaque ? "opaque" : "translucent");

    if (debug::renderTree) {
        Log::Info(Event::Render, "%*s%s {", indent++ * 4, "",
                  pass == RenderPass::Opaque ? "opaque" : "translucent");
    }

    for (; it != end; ++it, i += increment) {
        currentLayer = i;

        const auto& item = *it;
        const RenderLayer& layer = item.layer;

        if (!layer.hasRenderPass(pass))
            continue;

        if (layer.is<RenderBackgroundLayer>()) {
            MBGL_DEBUG_GROUP(context, "background");
            renderBackground(parameters, *layer.as<RenderBackgroundLayer>());
        } else if (layer.is<RenderFillExtrusionLayer>()) {
            const auto size = context.viewport.getCurrentValue().size;

            if (!extrusionTexture || extrusionTexture->getSize() != size) {
                extrusionTexture = OffscreenTexture(context, size, OffscreenTextureAttachment::Depth);
            }

            extrusionTexture->bind();

            context.setStencilMode(gl::StencilMode::disabled());
            context.setDepthMode(depthModeForSublayer(0, gl::DepthMode::ReadWrite));
            context.clear(Color{ 0.0f, 0.0f, 0.0f, 0.0f }, 1.0f, {});

            renderItem(parameters, item);

            parameters.view.bind();
            context.bindTexture(extrusionTexture->getTexture());

            mat4 viewportMat;
            matrix::ortho(viewportMat, 0, size.width, size.height, 0, 0, 1);

            const Properties<>::PossiblyEvaluated properties;

            parameters.programs.extrusionTexture.draw(
                context, gl::Triangles(), gl::DepthMode::disabled(), gl::StencilMode::disabled(),
                colorModeForRenderPass(),
                ExtrusionTextureProgram::UniformValues{
                    uniforms::u_matrix::Value{ viewportMat }, uniforms::u_world::Value{ size },
                    uniforms::u_image::Value{ 0 },
                    uniforms::u_opacity::Value{ layer.as<RenderFillExtrusionLayer>()
                                                    ->evaluated.get<FillExtrusionOpacity>() } },
                extrusionTextureDrawable.vertices,
                extrusionTextureDrawable.indices,
                extrusionTextureDrawable.segments,
                ExtrusionTextureProgram::PaintPropertyBinders{ properties, 0 }, properties,
                state.getZoom());
        } else {
            renderItem(parameters, item);
        }
    }

    if (debug::renderTree) {
        Log::Info(Event::Render, "%*s%s", --indent * 4, "", "}");
    }
}

void Painter::renderItem(PaintParameters& parameters, const RenderItem& item) {
    RenderLayer& layer = item.layer;
    MBGL_DEBUG_GROUP(context, layer.getID());
    layer.render(*this, parameters, item.source);
}

mat4 Painter::matrixForTile(const UnwrappedTileID& tileID) {
    mat4 matrix;
    state.matrixFor(matrix, tileID);
    matrix::multiply(matrix, projMatrix, matrix);
    return matrix;
}

gl::DepthMode Painter::depthModeForSublayer(uint8_t n, gl::DepthMode::Mask mask) const {
    float nearDepth = ((1 + currentLayer) * numSublayers + n) * depthEpsilon;
    float farDepth = nearDepth + depthRangeSize;
    return gl::DepthMode { gl::DepthMode::LessEqual, mask, { nearDepth, farDepth } };
}

gl::StencilMode Painter::stencilModeForClipping(const ClipID& id) const {
    return gl::StencilMode {
        gl::StencilMode::Equal { static_cast<uint32_t>(id.mask.to_ulong()) },
        static_cast<int32_t>(id.reference.to_ulong()),
        0,
        gl::StencilMode::Keep,
        gl::StencilMode::Keep,
        gl::StencilMode::Replace
    };
}

gl::ColorMode Painter::colorModeForRenderPass() const {
    if (paintMode() == PaintMode::Overdraw) {
        const float overdraw = 1.0f / 8.0f;
        return gl::ColorMode {
            gl::ColorMode::Add {
                gl::ColorMode::ConstantColor,
                gl::ColorMode::One
            },
            Color { overdraw, overdraw, overdraw, 0.0f },
            gl::ColorMode::Mask { true, true, true, true }
        };
    } else if (pass == RenderPass::Translucent) {
        return gl::ColorMode::alphaBlended();
    } else {
        return gl::ColorMode::unblended();
    }
}

} // namespace mbgl
