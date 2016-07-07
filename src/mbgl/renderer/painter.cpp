#include <mbgl/renderer/painter.hpp>
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

#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/shader/outline_shader.hpp>
#include <mbgl/shader/outlinepattern_shader.hpp>
#include <mbgl/shader/line_shader.hpp>
#include <mbgl/shader/linesdf_shader.hpp>
#include <mbgl/shader/linepattern_shader.hpp>
#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/shader/sdf_shader.hpp>
#include <mbgl/shader/collision_box_shader.hpp>
#include <mbgl/shader/circle_shader.hpp>

#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat3.hpp>
#include <mbgl/util/string.hpp>

#if defined(DEBUG)
#include <mbgl/util/stopwatch.hpp>
#endif

#include <cassert>
#include <algorithm>
#include <iostream>

namespace mbgl {

using namespace style;

Painter::Painter(const TransformState& state_,
                 gl::ObjectStore& store_)
    : state(state_), store(store_) {
    gl::debugging::enable();

    shader.plain = std::make_unique<PlainShader>(store);
    shader.outline = std::make_unique<OutlineShader>(store);
    shader.outlinePattern = std::make_unique<OutlinePatternShader>(store);
    shader.line = std::make_unique<LineShader>(store);
    shader.linesdf = std::make_unique<LineSDFShader>(store);
    shader.linepattern = std::make_unique<LinepatternShader>(store);
    shader.pattern = std::make_unique<PatternShader>(store);
    shader.icon = std::make_unique<IconShader>(store);
    shader.raster = std::make_unique<RasterShader>(store);
    shader.sdfGlyph = std::make_unique<SDFShader>(store);
    shader.sdfIcon = std::make_unique<SDFShader>(store);
    shader.collisionBox = std::make_unique<CollisionBoxShader>(store);
    shader.circle = std::make_unique<CircleShader>(store);

    overdrawShader.plain = std::make_unique<PlainShader>(store, Shader::Overdraw);
    overdrawShader.outline = std::make_unique<OutlineShader>(store, Shader::Overdraw);
    overdrawShader.outlinePattern = std::make_unique<OutlinePatternShader>(store, Shader::Overdraw);
    overdrawShader.line = std::make_unique<LineShader>(store, Shader::Overdraw);
    overdrawShader.linesdf = std::make_unique<LineSDFShader>(store, Shader::Overdraw);
    overdrawShader.linepattern = std::make_unique<LinepatternShader>(store, Shader::Overdraw);
    overdrawShader.pattern = std::make_unique<PatternShader>(store, Shader::Overdraw);
    overdrawShader.icon = std::make_unique<IconShader>(store, Shader::Overdraw);
    overdrawShader.raster = std::make_unique<RasterShader>(store, Shader::Overdraw);
    overdrawShader.sdfGlyph = std::make_unique<SDFShader>(store, Shader::Overdraw);
    overdrawShader.sdfIcon = std::make_unique<SDFShader>(store, Shader::Overdraw);
    overdrawShader.circle = std::make_unique<CircleShader>(store, Shader::Overdraw);

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

    // The native matrix is a 1:1 matrix that paints the coordinates at the
    // same screen position as the vertex specifies.
    matrix::identity(nativeMatrix);
    matrix::multiply(nativeMatrix, projMatrix, nativeMatrix);

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

        drawClippingMasks(generator.getStencils());
    }

    if (frame.debugOptions & MapDebugOptions::StencilClip) {
        renderClipMasks();
        return;
    }

    // Actually render the layers
    if (debug::renderTree) { Log::Info(Event::Render, "{"); indent++; }

    // TODO: Correctly compute the number of layers recursively beforehand.
    depthRangeSize = 1 - (order.size() + 2) * numSublayers * depthEpsilon;

    // - OPAQUE PASS -------------------------------------------------------------------------------
    // Render everything top-to-bottom by using reverse iterators. Render opaque objects first.
    renderPass(RenderPass::Opaque,
               order.rbegin(), order.rend(),
               0, 1);

    // - TRANSLUCENT PASS --------------------------------------------------------------------------
    // Make a second pass, rendering translucent objects. This time, we render bottom-to-top.
    renderPass(RenderPass::Translucent,
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
void Painter::renderPass(RenderPass pass_,
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
            renderBackground(*layer.as<BackgroundLayer>());
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
            item.bucket->render(*this, layer, item.tile->id, item.tile->matrix);
        }
    }

    if (debug::renderTree) {
        Log::Info(Event::Render, "%*s%s", --indent * 4, "", "}");
    }
}

mat4 Painter::translatedMatrix(const mat4& matrix,
                               const std::array<float, 2>& translation,
                               const UnwrappedTileID& id,
                               TranslateAnchorType anchor) {
    if (translation[0] == 0 && translation[1] == 0) {
        return matrix;
    } else {
        mat4 vtxMatrix;
        if (anchor == TranslateAnchorType::Viewport) {
            const double sin_a = std::sin(-state.getAngle());
            const double cos_a = std::cos(-state.getAngle());
            matrix::translate(vtxMatrix, matrix,
                    id.pixelsToTileUnits(translation[0] * cos_a - translation[1] * sin_a, state.getZoom()),
                    id.pixelsToTileUnits(translation[0] * sin_a + translation[1] * cos_a, state.getZoom()),
                    0);
        } else {
            matrix::translate(vtxMatrix, matrix,
                    id.pixelsToTileUnits(translation[0], state.getZoom()),
                    id.pixelsToTileUnits(translation[1], state.getZoom()),
                    0);
        }

        return vtxMatrix;
    }
}

void Painter::setDepthSublayer(int n) {
    float nearDepth = ((1 + currentLayer) * numSublayers + n) * depthEpsilon;
    float farDepth = nearDepth + depthRangeSize;
    config.depthRange = { nearDepth, farDepth };
}

} // namespace mbgl
