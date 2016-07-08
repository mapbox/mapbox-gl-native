#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/shader/sdf_shader.hpp>
#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/collision_box_shader.hpp>

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

void SymbolBucket::upload(gl::ObjectStore& store, gl::Config& config) {
    if (hasTextData()) {
        text.vertices.upload(store, config);
        text.triangles.upload(store, config);
    }
    if (hasIconData()) {
        icon.vertices.upload(store, config);
        icon.triangles.upload(store, config);
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

void SymbolBucket::drawGlyphs(SDFShader& shader,
                              gl::ObjectStore& store,
                              gl::Config& config,
                              PaintMode paintMode) {
    GLbyte* vertex_index = BUFFER_OFFSET_0;
    GLbyte* elements_index = BUFFER_OFFSET_0;
    for (auto& group : text.groups) {
        assert(group);
        group->array[paintMode == PaintMode::Overdraw ? 1 : 0].bind(
            shader, text.vertices, text.triangles, vertex_index, store, config);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group->vertex_length * text.vertices.itemSize;
        elements_index += group->elements_length * text.triangles.itemSize;
    }
}

void SymbolBucket::drawIcons(SDFShader& shader,
                             gl::ObjectStore& store,
                             gl::Config& config,
                             PaintMode paintMode) {
    GLbyte* vertex_index = BUFFER_OFFSET_0;
    GLbyte* elements_index = BUFFER_OFFSET_0;
    for (auto& group : icon.groups) {
        assert(group);
        group->array[paintMode == PaintMode::Overdraw ? 1 : 0].bind(
            shader, icon.vertices, icon.triangles, vertex_index, store, config);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group->vertex_length * icon.vertices.itemSize;
        elements_index += group->elements_length * icon.triangles.itemSize;
    }
}

void SymbolBucket::drawIcons(IconShader& shader,
                             gl::ObjectStore& store,
                             gl::Config& config,
                             PaintMode paintMode) {
    GLbyte* vertex_index = BUFFER_OFFSET_0;
    GLbyte* elements_index = BUFFER_OFFSET_0;
    for (auto& group : icon.groups) {
        assert(group);
        group->array[paintMode == PaintMode::Overdraw ? 3 : 2].bind(
            shader, icon.vertices, icon.triangles, vertex_index, store, config);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group->vertex_length * icon.vertices.itemSize;
        elements_index += group->elements_length * icon.triangles.itemSize;
    }
}

void SymbolBucket::drawCollisionBoxes(CollisionBoxShader& shader,
                                      gl::ObjectStore& store,
                                      gl::Config& config) {
    GLbyte* vertex_index = BUFFER_OFFSET_0;
    for (auto& group : collisionBox.groups) {
        group->array[0].bind(shader, collisionBox.vertices, vertex_index, store, config);
        MBGL_CHECK_ERROR(glDrawArrays(GL_LINES, 0, group->vertex_length));
    }
}

} // namespace mbgl
