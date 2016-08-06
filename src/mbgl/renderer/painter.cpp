#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>

#include <mbgl/style/source.hpp>
#include <mbgl/style/source_impl.hpp>

#include <mbgl/platform/log.hpp>
#include <mbgl/gl/debugging.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/style/layer_impl.hpp>

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>

#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>

#include <mbgl/shader/shaders.hpp>

#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat3.hpp>
#include <mbgl/util/string.hpp>

#include <cassert>
#include <algorithm>
#include <iostream>

namespace mbgl {

using namespace style;

Painter::Painter(const TransformState& state_,
                 gl::ObjectStore& store_)
    : state(state_), store(store_) {
    gl::debugging::enable();

    shaders = std::make_unique<Shaders>(store);
#ifndef NDEBUG
    overdrawShaders = std::make_unique<Shaders>(store, Shader::Overdraw);
#endif

    // Reset GL values
    config.setDirty();
    config.reset();
}

Painter::~Painter() = default;

bool Painter::needsAnimation() const {
    return frameHistory.needsAnimation(util::DEFAULT_FADE_DURATION);
}

void Painter::setClipping(const ClipID& clip) {
    const GLint ref = (GLint)clip.reference.to_ulong();
    const GLuint mask = (GLuint)clip.mask.to_ulong();
    config.stencilFunc = { GL_EQUAL, ref, mask };
}

void Painter::render(const Style& style, const FrameData& frame_, SpriteAtlas& annotationSpriteAtlas) {
    frame = frame_;

    PaintParameters parameters {
#ifndef NDEBUG
        isOverdraw() ? *overdrawShaders : *shaders
#else
        *shaders
#endif
    };

    glyphAtlas = style.glyphAtlas.get();
    spriteAtlas = style.spriteAtlas.get();
    lineAtlas = style.lineAtlas.get();

    RenderData renderData = style.getRenderData(frame.debugOptions);
    const std::vector<RenderItem>& order = renderData.order;
    const std::set<Source*>& sources = renderData.sources;
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

        tileStencilBuffer.upload(store);
        rasterBoundsBuffer.upload(store);
        tileBorderBuffer.upload(store);
        spriteAtlas->upload(store, config, 0);
        lineAtlas->upload(store, config, 0);
        glyphAtlas->upload(store, config, 0);
        frameHistory.upload(store, config, 0);
        annotationSpriteAtlas.upload(store, config, 0);

        for (const auto& item : order) {
            if (item.bucket && item.bucket->needsUpload()) {
                item.bucket->upload(store, config);
            }
        }
    }

    // - CLEAR -------------------------------------------------------------------------------------
    // Renders the backdrop of the OpenGL view. This also paints in areas where we don't have any
    // tiles whatsoever.
    {
        MBGL_DEBUG_GROUP("clear");
        config.stencilFunc.reset();
        config.stencilTest = GL_TRUE;
        config.stencilMask = 0xFF;
        config.depthTest = GL_FALSE;
        config.depthMask = GL_TRUE;
        config.colorMask = { GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE };

        if (isOverdraw()) {
            config.blend = GL_TRUE;
            config.blendFunc = { GL_CONSTANT_COLOR, GL_ONE };
            const float overdraw = 1.0f / 8.0f;
            config.blendColor = { overdraw, overdraw, overdraw, 0.0f };
            config.clearColor = Color::black();
        } else {
            config.clearColor = background;
        }
        config.clearStencil = 0;
        config.clearDepth = 1;
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

#ifndef NDEBUG
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

    // TODO: Find a better way to unbind VAOs after we're done with them without introducing
    // unnecessary bind(0)/bind(N) sequences.
    {
        MBGL_DEBUG_GROUP("cleanup");

        config.activeTexture = 1;
        config.texture[1] = 0;
        config.activeTexture = 0;
        config.texture[0] = 0;

        MBGL_CHECK_ERROR(VertexArrayObject::Unbind());
    }

    if (frame.contextMode == GLContextMode::Shared) {
        config.setDirty();
    }
}

template <class Iterator>
void Painter::renderPass(PaintParameters& parameters,
                         RenderPass pass_,
                         Iterator it, Iterator end,
                         GLsizei i, int8_t increment) {
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

        if (isOverdraw()) {
            config.blend = GL_TRUE;
        } else if (pass == RenderPass::Translucent) {
            config.blendFunc.reset();
            config.blend = GL_TRUE;
        } else {
            config.blend = GL_FALSE;
        }

        config.colorMask = { GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE };
        config.stencilMask = 0x0;

        if (layer.is<BackgroundLayer>()) {
            MBGL_DEBUG_GROUP("background");
            renderBackground(parameters, *layer.as<BackgroundLayer>());
        } else if (layer.is<CustomLayer>()) {
            MBGL_DEBUG_GROUP(layer.baseImpl->id + " - custom");
            VertexArrayObject::Unbind();
            layer.as<CustomLayer>()->impl->render(state);
            config.setDirty();
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
    config.depthRange = { nearDepth, farDepth };
}

} // namespace mbgl
