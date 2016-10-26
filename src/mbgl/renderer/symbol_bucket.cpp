#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/shader/symbol_sdf_shader.hpp>
#include <mbgl/shader/symbol_icon_shader.hpp>
#include <mbgl/shader/collision_box_shader.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

using namespace style;

SymbolBucket::SymbolBucket(const MapMode mode_,
                           style::SymbolLayoutProperties layout_,
                           bool sdfIcons_,
                           bool iconsNeedLinear_)
    : mode(mode_),
      layout(std::move(layout_)),
      sdfIcons(sdfIcons_),
      iconsNeedLinear(iconsNeedLinear_) {
}

void SymbolBucket::upload(gl::Context& context) {
    if (hasTextData()) {
        text.vertexBuffer = context.createVertexBuffer(std::move(text.vertices));
        text.indexBuffer = context.createIndexBuffer(std::move(text.triangles));
    }

    if (hasIconData()) {
        icon.vertexBuffer = context.createVertexBuffer(std::move(icon.vertices));
        icon.indexBuffer = context.createIndexBuffer(std::move(icon.triangles));
    }

    if (hasCollisionBoxData()) {
        collisionBox.vertexBuffer = context.createVertexBuffer(std::move(collisionBox.vertices));
    }

    uploaded = true;
}

void SymbolBucket::render(Painter& painter,
                          PaintParameters& parameters,
                          const Layer& layer,
                          const RenderTile& tile) {
    painter.renderSymbol(parameters, *this, *layer.as<SymbolLayer>(), tile);
}

bool SymbolBucket::hasData() const {
    assert(false); // Should be calling SymbolLayout::hasSymbolInstances() instead.
    return false;
}

bool SymbolBucket::hasTextData() const {
    return !text.groups.empty();
}

bool SymbolBucket::hasIconData() const {
    return !icon.groups.empty();
}

bool SymbolBucket::hasCollisionBoxData() const {
    return !collisionBox.groups.empty();
}

bool SymbolBucket::needsClipping() const {
    return mode == MapMode::Still;
}

void SymbolBucket::drawGlyphs(SymbolSDFShader& shader,
                              gl::Context& context,
                              PaintMode paintMode) {
    GLbyte* vertex_index = BUFFER_OFFSET_0;
    GLbyte* elements_index = BUFFER_OFFSET_0;
    for (auto& group : text.groups) {
        group.getVAO(shader, paintMode).bind(
            shader, *text.vertexBuffer, *text.indexBuffer, vertex_index, context);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(group.indexLength * 3), GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group.vertexLength * text.vertexBuffer->vertexSize;
        elements_index += group.indexLength * text.indexBuffer->primitiveSize;
    }
}

void SymbolBucket::drawIcons(SymbolSDFShader& shader,
                             gl::Context& context,
                             PaintMode paintMode) {
    GLbyte* vertex_index = BUFFER_OFFSET_0;
    GLbyte* elements_index = BUFFER_OFFSET_0;
    for (auto& group : icon.groups) {
        group.getVAO(shader, paintMode).bind(
            shader, *icon.vertexBuffer, *icon.indexBuffer, vertex_index, context);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(group.indexLength * 3), GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group.vertexLength * icon.vertexBuffer->vertexSize;
        elements_index += group.indexLength * icon.indexBuffer->primitiveSize;
    }
}

void SymbolBucket::drawIcons(SymbolIconShader& shader,
                             gl::Context& context,
                             PaintMode paintMode) {
    GLbyte* vertex_index = BUFFER_OFFSET_0;
    GLbyte* elements_index = BUFFER_OFFSET_0;
    for (auto& group : icon.groups) {
        group.getVAO(shader, paintMode).bind(
            shader, *icon.vertexBuffer, *icon.indexBuffer, vertex_index, context);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(group.indexLength * 3), GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group.vertexLength * icon.vertexBuffer->vertexSize;
        elements_index += group.indexLength * icon.indexBuffer->primitiveSize;
    }
}

void SymbolBucket::drawCollisionBoxes(CollisionBoxShader& shader,
                                      gl::Context& context) {
    GLbyte* vertex_index = BUFFER_OFFSET_0;
    for (auto& group : collisionBox.groups) {
        group.getVAO(shader, PaintMode::Regular).bind(
            shader, *collisionBox.vertexBuffer, vertex_index, context);
        MBGL_CHECK_ERROR(glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(group.vertexLength)));
    }
}

} // namespace mbgl
