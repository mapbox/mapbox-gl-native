#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>

#include <mbgl/style/source.hpp>
#include <mbgl/style/source_impl.hpp>

#include <mbgl/map/view.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/gl/debugging.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/style/layer_impl.hpp>

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>

#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/text/glyph_atlas.hpp>

#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/programs/programs.hpp>

#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat3.hpp>
#include <mbgl/util/string.hpp>

#include <mbgl/util/offscreen_texture.hpp>

#include <mbgl/util/stopwatch.hpp>

#include <cassert>
#include <algorithm>
#include <iostream>
#include <unordered_set>

namespace mbgl {

using namespace style;

static gl::VertexVector<FillLayoutVertex> tileVertices() {
    gl::VertexVector<FillLayoutVertex> result;
    result.emplace_back(FillProgram::layoutVertex({ 0,            0 }));
    result.emplace_back(FillProgram::layoutVertex({ util::EXTENT, 0 }));
    result.emplace_back(FillProgram::layoutVertex({ 0, util::EXTENT }));
    result.emplace_back(FillProgram::layoutVertex({ util::EXTENT, util::EXTENT }));
    return result;
}

static gl::IndexVector<gl::Triangles> tileTriangleIndices() {
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
    result.emplace_back(RasterProgram::layoutVertex({ util::EXTENT, 0 }, { 32767, 0 }));
    result.emplace_back(RasterProgram::layoutVertex({ 0, util::EXTENT }, { 0, 32767 }));
    result.emplace_back(RasterProgram::layoutVertex({ util::EXTENT, util::EXTENT }, { 32767, 32767 }));
    return result;
}

static gl::VertexVector<ExtrusionTextureLayoutVertex>
extrusionTextureVertices(const TransformState& state) {
    gl::VertexVector<ExtrusionTextureLayoutVertex> result;
    result.emplace_back(ExtrusionTextureProgram::layoutVertex({ 0, 0 }));
    result.emplace_back(
        ExtrusionTextureProgram::layoutVertex({ static_cast<int16_t>(state.getSize().width), 0 }));
    result.emplace_back(
        ExtrusionTextureProgram::layoutVertex({ 0, static_cast<int16_t>(state.getSize().height) }));
    result.emplace_back(
        ExtrusionTextureProgram::layoutVertex({ static_cast<int16_t>(state.getSize().width),
                                                static_cast<short>(state.getSize().height) }));
    return result;
}


Painter::Painter(gl::Context& context_,
                 const TransformState& state_,
                 float pixelRatio,
                 const std::string& programCacheDir)
    : context(context_),
      state(state_),
      tileVertexBuffer(context.createVertexBuffer(tileVertices())),
      rasterVertexBuffer(context.createVertexBuffer(rasterVertices())),
      extrusionTextureVertexBuffer(context.createVertexBuffer(extrusionTextureVertices(state_))),
      tileTriangleIndexBuffer(context.createIndexBuffer(tileTriangleIndices())),
      tileBorderIndexBuffer(context.createIndexBuffer(tileLineStripIndices())) {

    tileTriangleSegments.emplace_back(0, 0, 4, 6);
    tileBorderSegments.emplace_back(0, 0, 4, 5);
    rasterSegments.emplace_back(0, 0, 4, 6);
    extrusionTextureSegments.emplace_back(0, 0, 4, 6); // TODO i have no idea what these numbers are

    programs = std::make_unique<Programs>(context,
                                          ProgramParameters{ pixelRatio, false, programCacheDir });
#ifndef NDEBUG
    overdrawPrograms =
        std::make_unique<Programs>(context, ProgramParameters{ pixelRatio, true, programCacheDir });
#endif
}

Painter::~Painter() = default;

bool Painter::needsAnimation() const {
    return frameHistory.needsAnimation(util::DEFAULT_FADE_DURATION);
}

void Painter::cleanup() {
    context.performCleanup();
}

void Painter::render(const Style& style, const FrameData& frame_, View& view, SpriteAtlas& annotationSpriteAtlas) {
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

    glyphAtlas = style.glyphAtlas.get();
    spriteAtlas = style.spriteAtlas.get();
    lineAtlas = style.lineAtlas.get();

    RenderData renderData = style.getRenderData(frame.debugOptions, state.getAngle());
    const std::vector<RenderItem>& order = renderData.order;
    const std::unordered_set<Source*>& sources = renderData.sources;

    // Update the default matrices to the current viewport dimensions.
    state.getProjMatrix(projMatrix);

    pixelsToGLUnits = {{ 2.0f  / state.getSize().width, -2.0f / state.getSize().height }};
    if (state.getViewportMode() == ViewportMode::FlippedY) {
        pixelsToGLUnits[1] *= -1;
    }

    frameHistory.record(frame.timePoint, state.getZoom(),
        frame.mapMode == MapMode::Continuous ? util::DEFAULT_FADE_DURATION : Milliseconds(0));


    // - UPLOAD PASS -------------------------------------------------------------------------------
    // Uploads all required buffers and images before we do any actual rendering.
    {
        MBGL_DEBUG_GROUP(context, "upload");

        spriteAtlas->upload(context, 0);

        lineAtlas->upload(context, 0);
        glyphAtlas->upload(context, 0);
        frameHistory.upload(context, 0);
        annotationSpriteAtlas.upload(context, 0);

        for (const auto& item : order) {
            for (const auto& tileRef : item.tiles) {
                const auto& bucket = tileRef.get().tile.getBucket(item.layer);
                if (bucket && bucket->needsUpload()) {
                    bucket->upload(context);
                }
            }
        }
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
        algorithm::ClipIDGenerator generator;
        for (const auto& source : sources) {
            source->baseImpl->startRender(generator, projMatrix, state);
        }

        MBGL_DEBUG_GROUP(context, "clipping masks");

        for (const auto& stencil : generator.getStencils()) {
            MBGL_DEBUG_GROUP(context, std::string{ "mask: " } + util::toString(stencil.first));
            renderClippingMask(stencil.first, stencil.second);
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

    // TODO: Correctly compute the number of layers recursively beforehand.
    // TODO TODO : this actually sounds like the one-order-item-per-layer might *fix* the above todo. Y/N?
    depthRangeSize = 1 - (order.size() + 2) * numSublayers * depthEpsilon;

    // - OPAQUE PASS -------------------------------------------------------------------------------
    // Render everything top-to-bottom by using reverse iterators. Render opaque objects first.
    renderPass(parameters,
               RenderPass::Opaque,
               style,
               order.rbegin(), order.rend(),
               0, 1);

    // - TRANSLUCENT PASS --------------------------------------------------------------------------
    // Make a second pass, rendering translucent objects. This time, we render bottom-to-top.
    renderPass(parameters,
               RenderPass::Translucent,
               style,
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
            source->baseImpl->finishRender(*this);
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
                         const Style& style,
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
        const Layer& layer = item.layer;

        if (!layer.baseImpl->hasRenderPass(pass))
            continue;

        auto renderTiles = [this, &item, &layer, &parameters, &style]() {
            for (auto& tileRef : item.tiles) {
                auto& tile = tileRef.get();
                MBGL_DEBUG_GROUP(layer.baseImpl->id + " - " + util::toString(tile.id));
                auto bucket = tile.tile.getBucket(layer);
                if (bucket) {
                    bucket->render(*this, parameters, layer, tile, style);
                }
            }
        };

        if (layer.is<BackgroundLayer>()) {
            MBGL_DEBUG_GROUP(context, "background");
            renderBackground(parameters, *layer.as<BackgroundLayer>());
        } else if (layer.is<CustomLayer>()) {
            MBGL_DEBUG_GROUP(context, layer.baseImpl->id + " - custom");

            // Reset GL state to a known state so the CustomLayer always has a clean slate.
            context.vertexArrayObject = 0;
            context.setDepthMode(depthModeForSublayer(0, gl::DepthMode::ReadOnly));
            context.setStencilMode(gl::StencilMode::disabled());
            context.setColorMode(colorModeForRenderPass());

            layer.as<CustomLayer>()->impl->render(state);

            // Reset the view back to our original one, just in case the CustomLayer changed
            // the viewport or Framebuffer.
            parameters.view.bind();
            context.setDirtyState();
        } else if (layer.is<FillExtrusionLayer>()) {
////            const auto size = state.getSize();
//            const auto size_ = context.viewport.getCurrentValue().size;
//            Size size{ size_.width / 2, size_.height / 2 };
//
//            // i think maybe there's a depth buffering issue at tile boundaries (?) — perhaps not using the same depth buffer?
//
//            OffscreenTexture texture(context, size);
//            texture.bindRenderbuffers();
//
//            context.setStencilMode(gl::StencilMode::disabled());
//            context.setDepthMode(depthModeForSublayer(0, gl::DepthMode::ReadWrite));
//            context.clear(Color{ 0.0f, 0.0f, 0.0f, 0.0f }, 1.0f, {});

            renderTiles();
//
//            parameters.view.bind();
//
//            mat4 mat;
//            matrix::ortho(mat, 0, size.width, size.height, 0, 0, 1);
//            const FillExtrusionPaintProperties::Evaluated properties{};
//
//            parameters.programs.extrusionTexture.draw(context,
//                                                      gl::Triangles(),
//                                                      gl::DepthMode::disabled(),
//                                                      gl::StencilMode::disabled(),
//                                                      colorModeForRenderPass(),
//                                                      ExtrusionTextureProgram::UniformValues{
//                                                          uniforms::u_matrix::Value{ mat },
//                                                          uniforms::u_world::Value{ size },
//                                                          uniforms::u_image::Value{ 0 }, // view.getTexture() ? no — but follow up on whether could be variable or if it's always safe to attach to 0 unit
//                                                          uniforms::u_opacity::Value{ 0.9 } // TODO implement parsing from style
//                                                      },
//                                                      extrusionTextureVertexBuffer,
//                                                      tileTriangleIndexBuffer,
//                                                      extrusionTextureSegments,
//                                                      ExtrusionTextureProgram::PaintPropertyBinders{ properties, 0 },
//                                                      properties,
//                                                      state.getZoom());


//            const auto size = state.getSize();
////            const auto size = Size{ _size.width - (_size.width % 4), _size.height - (_size.height % 4) };
//
//            OffscreenTexture texture(context, size);
//            texture.bindRenderbuffers();
//
//            context.setStencilMode(gl::StencilMode::disabled());
//            context.setDepthMode(depthModeForSublayer(0, gl::DepthMode::ReadWrite));
//
//            context.clear(Color{ 0.0f, 0.0f, 0.0f, 0.0f }, 1.0f, {});
//
//            renderTiles();
//
//            parameters.view.bind();
//
//            mat4 mat;
//            matrix::ortho(mat, 0, size.width, size.height, 0, 0, 1);
//
//            const FillExtrusionPaintProperties::Evaluated properties{};
//
//            // DEBUG: trying to render texture to map
////            context.activeTexture = 0;
////            MBGL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLenum>(GL_RGBA), size.width,
////                                          size.height, 0, static_cast<GLenum>(GL_RGBA), GL_UNSIGNED_BYTE,
////                                          &texture.getTexture()));
//
//
//            parameters.programs.extrusionTexture.draw(context,
//                                                      gl::Triangles(),
//                                                      gl::DepthMode::disabled(),
//                                                      gl::StencilMode::disabled(),
//                                                      colorModeForRenderPass(),
//                                                      ExtrusionTextureProgram::UniformValues{
//                                                          uniforms::u_matrix::Value{ mat },
//                                                          uniforms::u_world::Value{ size },
//                                                          uniforms::u_image::Value{ 0 }, // view.getTexture() ? no — but follow up on whether could be variable or if it's always safe to attach to 0 unit
//                                                          uniforms::u_opacity::Value{ 0.9 } // TODO implement parsing from style
//                                                      },
//                                                      extrusionTextureVertexBuffer,
//                                                      tileTriangleIndexBuffer, // we reuse the same simple index buffer
//                                                      extrusionTextureSegments,
//                                                      ExtrusionTextureProgram::PaintPropertyBinders{ properties, 0 },
//                                                      FillExtrusionPaintProperties::Evaluated{},
//                                                      state.getZoom());

        } else {
            renderTiles();
        }
    }

    if (debug::renderTree) {
        Log::Info(Event::Render, "%*s%s", --indent * 4, "", "}");
    }
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
