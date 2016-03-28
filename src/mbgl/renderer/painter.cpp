#include <mbgl/renderer/painter.hpp>

#include <mbgl/source/source.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/map/map_context.hpp>
#include <mbgl/map/map_data.hpp>

#include <mbgl/platform/log.hpp>
#include <mbgl/gl/debugging.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_render_parameters.hpp>

#include <mbgl/layer/background_layer.hpp>
#include <mbgl/layer/custom_layer.hpp>

#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>

#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/shader/outline_shader.hpp>
#include <mbgl/shader/line_shader.hpp>
#include <mbgl/shader/linesdf_shader.hpp>
#include <mbgl/shader/linepattern_shader.hpp>
#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/shader/sdf_shader.hpp>
#include <mbgl/shader/dot_shader.hpp>
#include <mbgl/shader/box_shader.hpp>
#include <mbgl/shader/circle_shader.hpp>

#include <mbgl/util/constants.hpp>

#if defined(DEBUG)
#include <mbgl/util/stopwatch.hpp>
#endif

#include <cassert>
#include <algorithm>
#include <iostream>

using namespace mbgl;

Painter::Painter(MapData& data_, TransformState& state_, gl::GLObjectStore& glObjectStore_)
    : data(data_),
      state(state_),
      glObjectStore(glObjectStore_) {
    gl::debugging::enable();

    plainShader = std::make_unique<PlainShader>(glObjectStore);
    outlineShader = std::make_unique<OutlineShader>(glObjectStore);
    lineShader = std::make_unique<LineShader>(glObjectStore);
    linesdfShader = std::make_unique<LineSDFShader>(glObjectStore);
    linepatternShader = std::make_unique<LinepatternShader>(glObjectStore);
    patternShader = std::make_unique<PatternShader>(glObjectStore);
    iconShader = std::make_unique<IconShader>(glObjectStore);
    rasterShader = std::make_unique<RasterShader>(glObjectStore);
    sdfGlyphShader = std::make_unique<SDFGlyphShader>(glObjectStore);
    sdfIconShader = std::make_unique<SDFIconShader>(glObjectStore);
    dotShader = std::make_unique<DotShader>(glObjectStore);
    collisionBoxShader = std::make_unique<CollisionBoxShader>(glObjectStore);
    circleShader = std::make_unique<CircleShader>(glObjectStore);

    // Reset GL values
    config.reset();
}

Painter::~Painter() = default;

bool Painter::needsAnimation() const {
    return frameHistory.needsAnimation(util::DEFAULT_FADE_DURATION);
}

void Painter::prepareTile(const Tile& tile) {
    const GLint ref = (GLint)tile.clip.reference.to_ulong();
    const GLuint mask = (GLuint)tile.clip.mask.to_ulong();
    config.stencilFunc = { GL_EQUAL, ref, mask };
}

void Painter::render(const Style& style, const FrameData& frame_, SpriteAtlas& annotationSpriteAtlas) {
    frame = frame_;

    glyphAtlas = style.glyphAtlas.get();
    spriteAtlas = style.spriteAtlas.get();
    lineAtlas = style.lineAtlas.get();

    RenderData renderData = style.getRenderData();
    const std::vector<RenderItem>& order = renderData.order;
    const std::set<Source*>& sources = renderData.sources;
    const Color& background = renderData.backgroundColor;

    // Update the default matrices to the current viewport dimensions.
    state.getProjMatrix(projMatrix);

    // The extrusion matrix.
    matrix::ortho(extrudeMatrix, 0, state.getWidth(), state.getHeight(), 0, 0, -1);

    // The native matrix is a 1:1 matrix that paints the coordinates at the
    // same screen position as the vertex specifies.
    matrix::identity(nativeMatrix);
    matrix::multiply(nativeMatrix, projMatrix, nativeMatrix);

    // - UPLOAD PASS -------------------------------------------------------------------------------
    // Uploads all required buffers and images before we do any actual rendering.
    {
        MBGL_DEBUG_GROUP("upload");

        tileStencilBuffer.upload(glObjectStore);
        tileBorderBuffer.upload(glObjectStore);
        spriteAtlas->upload(glObjectStore);
        lineAtlas->upload(glObjectStore);
        glyphAtlas->upload(glObjectStore);
        annotationSpriteAtlas.upload(glObjectStore);

        for (const auto& item : order) {
            if (item.bucket && item.bucket->needsUpload()) {
                item.bucket->upload(glObjectStore);
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
        config.clearColor = { background[0], background[1], background[2], background[3] };
        config.clearStencil = 0;
        config.clearDepth = 1;
        MBGL_CHECK_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

    // - CLIPPING MASKS ----------------------------------------------------------------------------
    // Draws the clipping masks to the stencil buffer.
    {
        MBGL_DEBUG_GROUP("clip");

        // Update all clipping IDs.
        ClipIDGenerator generator;
        for (const auto& source : sources) {
            generator.update(source->getLoadedTiles());
            source->updateMatrices(projMatrix, state);
        }

        drawClippingMasks(generator.getStencils());
    }

    frameHistory.record(frame.timePoint, state.getZoom());

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
            source->finishRender(*this);
        }
    }

    // TODO: Find a better way to unbind VAOs after we're done with them without introducing
    // unnecessary bind(0)/bind(N) sequences.
    {
        MBGL_DEBUG_GROUP("cleanup");

        MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
        MBGL_CHECK_ERROR(VertexArrayObject::Unbind());
    }

    if (data.contextMode == GLContextMode::Shared) {
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
        const StyleLayer& layer = item.layer;

        if (!layer.hasRenderPass(pass))
            continue;

        if (pass == RenderPass::Translucent) {
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
            MBGL_DEBUG_GROUP(layer.id + " - custom");
            VertexArrayObject::Unbind();
            layer.as<CustomLayer>()->render(state);
            config.setDirty();
        } else {
            MBGL_DEBUG_GROUP(layer.id + " - " + std::string(item.tile->id));
            prepareTile(*item.tile);
            item.bucket->render(*this, layer, item.tile->id, item.tile->matrix);
        }
    }

    if (debug::renderTree) {
        Log::Info(Event::Render, "%*s%s", --indent * 4, "", "}");
    }
}

mat4 Painter::translatedMatrix(const mat4& matrix, const std::array<float, 2> &translation, const TileID &id, TranslateAnchorType anchor) {
    if (translation[0] == 0 && translation[1] == 0) {
        return matrix;
    } else {
        const double factor = double(1ll << id.sourceZ) * util::EXTENT / util::tileSize / state.getScale();

        mat4 vtxMatrix;
        if (anchor == TranslateAnchorType::Viewport) {
            const double sin_a = std::sin(-state.getAngle());
            const double cos_a = std::cos(-state.getAngle());
            matrix::translate(vtxMatrix, matrix,
                    factor * (translation[0] * cos_a - translation[1] * sin_a),
                    factor * (translation[0] * sin_a + translation[1] * cos_a),
                    0);
        } else {
            matrix::translate(vtxMatrix, matrix,
                    factor * translation[0],
                    factor * translation[1],
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
