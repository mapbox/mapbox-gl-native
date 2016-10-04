#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>

#include <mbgl/style/source.hpp>
#include <mbgl/style/source_impl.hpp>

#include <mbgl/platform/log.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/debugging.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/style/layer_impl.hpp>

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>

#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/text/glyph_atlas.hpp>

#include <mbgl/shader/shaders.hpp>

#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat3.hpp>
#include <mbgl/util/string.hpp>

#include <cassert>
#include <algorithm>
#include <iostream>
#include <unordered_set>

namespace mbgl {

using namespace style;

Painter::Painter(const TransformState& state_)
    : state(state_),
      tileTriangleVertexBuffer(context.createVertexBuffer(std::vector<FillVertex> {{
            { 0,            0 },
            { util::EXTENT, 0 },
            { 0, util::EXTENT },
            { util::EXTENT, 0 },
            { 0, util::EXTENT },
            { util::EXTENT, util::EXTENT }
      }})),
      tileLineStripVertexBuffer(context.createVertexBuffer(std::vector<FillVertex> {{
            { 0, 0 },
            { util::EXTENT, 0 },
            { util::EXTENT, util::EXTENT },
            { 0, util::EXTENT },
            { 0, 0 }
      }})),
      rasterVertexBuffer(context.createVertexBuffer(std::vector<RasterVertex> {{
            { 0, 0, 0, 0 },
            { util::EXTENT, 0, 32767, 0 },
            { 0, util::EXTENT, 0, 32767 },
            { util::EXTENT, util::EXTENT, 32767, 32767 }
      }})) {
#ifndef NDEBUG
    gl::debugging::enable();
#endif

    shaders = std::make_unique<Shaders>(context);
#ifndef NDEBUG
    overdrawShaders = std::make_unique<Shaders>(context, gl::Shader::Overdraw);
#endif

    // Reset GL values
    context.setDirtyState();
}

Painter::~Painter() = default;

bool Painter::needsAnimation() const {
    return frameHistory.needsAnimation(util::DEFAULT_FADE_DURATION);
}

void Painter::setClipping(const ClipID& clip) {
    const GLint ref = (GLint)clip.reference.to_ulong();
    const GLuint mask = (GLuint)clip.mask.to_ulong();
    context.stencilFunc = { gl::StencilTestFunction::Equal, ref, mask };
}

void Painter::cleanup() {
    context.performCleanup();
}

void Painter::render(const Style& style, const FrameData& frame_, SpriteAtlas& annotationSpriteAtlas) {
    if (frame.framebufferSize != frame_.framebufferSize) {
        context.viewport.setDefaultValue(
            { 0, 0, frame_.framebufferSize[0], frame_.framebufferSize[1] });
    }
    frame = frame_;

    PaintParameters parameters {
#ifndef NDEBUG
        paintMode() == PaintMode::Overdraw ? *overdrawShaders : *shaders
#else
        *shaders
#endif
    };

    glyphAtlas = style.glyphAtlas.get();
    spriteAtlas = style.spriteAtlas.get();
    lineAtlas = style.lineAtlas.get();

    RenderData renderData = style.getRenderData(frame.debugOptions);
    const std::vector<RenderItem>& order = renderData.order;
    const std::unordered_set<Source*>& sources = renderData.sources;
    const Color& background = renderData.backgroundColor;

    // Update the default matrices to the current viewport dimensions.
    state.getProjMatrix(projMatrix);

    pixelsToGLUnits = {{ 2.0f  / state.getWidth(), -2.0f / state.getHeight() }};
    if (state.getViewportMode() == ViewportMode::FlippedY) {
        pixelsToGLUnits[1] *= -1;
    }

    frameHistory.record(frame.timePoint, state.getZoom(),
        frame.mapMode == MapMode::Continuous ? util::DEFAULT_FADE_DURATION : Milliseconds(0));

    // - UPLOAD PASS -------------------------------------------------------------------------------
    // Uploads all required buffers and images before we do any actual rendering.
    {
        MBGL_DEBUG_GROUP("upload");

        spriteAtlas->upload(context, 0);
        lineAtlas->upload(context, 0);
        glyphAtlas->upload(context, 0);
        frameHistory.upload(context, 0);
        annotationSpriteAtlas.upload(context, 0);

        for (const auto& item : order) {
            if (item.bucket && item.bucket->needsUpload()) {
                item.bucket->upload(context);
            }
        }
    }

    // - CLEAR -------------------------------------------------------------------------------------
    // Renders the backdrop of the OpenGL view. This also paints in areas where we don't have any
    // tiles whatsoever.
    {
        MBGL_DEBUG_GROUP("clear");
        context.bindFramebuffer.reset();
        context.viewport.reset();
        context.stencilFunc.reset();
        context.stencilTest = true;
        context.stencilMask = 0xFF;
        context.depthTest = false;
        context.depthMask = true;
        context.colorMask = { true, true, true, true };

        if (paintMode() == PaintMode::Overdraw) {
            context.blend = true;
            context.blendFunc = { gl::BlendSourceFactor::ConstantColor,
                                  gl::BlendDestinationFactor::One };
            const float overdraw = 1.0f / 8.0f;
            context.blendColor = { overdraw, overdraw, overdraw, 0.0f };
            context.clearColor = Color::black();
        } else {
            context.clearColor = background;
        }
        context.clearStencil = 0;
        context.clearDepth = 1;
        MBGL_CHECK_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

    // - CLIPPING MASKS ----------------------------------------------------------------------------
    // Draws the clipping masks to the stencil buffer.
    {
        MBGL_DEBUG_GROUP("clip");

        // Update all clipping IDs.
        algorithm::ClipIDGenerator generator;
        for (const auto& source : sources) {
            source->baseImpl->startRender(generator, projMatrix, state);
        }

        drawClippingMasks(parameters, generator.getStencils());
    }

#if not MBGL_USE_GLES2 and not defined(NDEBUG)
    if (frame.debugOptions & MapDebugOptions::StencilClip) {
        renderClipMasks();
        return;
    }
#endif

    // Actually render the layers
    if (debug::renderTree) { Log::Info(Event::Render, "{"); indent++; }

    // TODO: Correctly compute the number of layers recursively beforehand.
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
               static_cast<GLsizei>(order.size()) - 1, -1);

    if (debug::renderTree) { Log::Info(Event::Render, "}"); indent--; }

    // - DEBUG PASS --------------------------------------------------------------------------------
    // Renders debug overlays.
    {
        MBGL_DEBUG_GROUP("debug");

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
        renderDepthBuffer();
    }
#endif

    // TODO: Find a better way to unbind VAOs after we're done with them without introducing
    // unnecessary bind(0)/bind(N) sequences.
    {
        MBGL_DEBUG_GROUP("cleanup");

        context.activeTexture = 1;
        context.texture[1] = 0;
        context.activeTexture = 0;
        context.texture[0] = 0;

        context.vertexArrayObject = 0;
    }

    if (frame.contextMode == GLContextMode::Shared) {
        context.setDirtyState();
    }
}

template <class Iterator>
void Painter::renderPass(PaintParameters& parameters,
                         RenderPass pass_,
                         Iterator it, Iterator end,
                         uint32_t i, int8_t increment) {
    pass = pass_;

    MBGL_DEBUG_GROUP(pass == RenderPass::Opaque ? "opaque" : "translucent");

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

        if (paintMode() == PaintMode::Overdraw) {
            context.blend = true;
        } else if (pass == RenderPass::Translucent) {
            context.blend = true;
            context.blendFunc = { gl::BlendSourceFactor::One,
                                  gl::BlendDestinationFactor::OneMinusSrcAlpha };
        } else {
            context.blend = false;
        }

        context.colorMask = { true, true, true, true };
        context.stencilMask = 0x0;

        if (layer.is<BackgroundLayer>()) {
            MBGL_DEBUG_GROUP("background");
            renderBackground(parameters, *layer.as<BackgroundLayer>());
        } else if (layer.is<CustomLayer>()) {
            MBGL_DEBUG_GROUP(layer.baseImpl->id + " - custom");
            context.vertexArrayObject = 0;
            context.depthFunc = gl::DepthTestFunction::LessEqual;
            context.depthTest = true;
            context.depthMask = false;
            context.stencilTest = false;
            setDepthSublayer(0);
            layer.as<CustomLayer>()->impl->render(state);
            context.setDirtyState();
            context.bindFramebuffer.reset();
            context.viewport.reset();
        } else {
            MBGL_DEBUG_GROUP(layer.baseImpl->id + " - " + util::toString(item.tile->id));
            if (item.bucket->needsClipping()) {
                setClipping(item.tile->clip);
            }
            item.bucket->render(*this, parameters, layer, *item.tile);
        }
    }

    if (debug::renderTree) {
        Log::Info(Event::Render, "%*s%s", --indent * 4, "", "}");
    }
}

void Painter::setDepthSublayer(int n) {
    float nearDepth = ((1 + currentLayer) * numSublayers + n) * depthEpsilon;
    float farDepth = nearDepth + depthRangeSize;
    context.depthRange = { nearDepth, farDepth };
}

} // namespace mbgl
