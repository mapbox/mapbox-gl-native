#include <llmr/renderer/icon_bucket.hpp>
#include <llmr/geometry/icon_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/geometry.hpp>

#include <llmr/renderer/painter.hpp>
#include <llmr/style/style.hpp>
#include <llmr/style/sprite.hpp>
#include <llmr/map/vector_tile.hpp>

#include <llmr/platform/gl.hpp>

#include <cassert>

struct geometry_too_long_exception : std::exception {};

using namespace llmr;

IconBucket::IconBucket(IconVertexBuffer& vertexBuffer,
                         const BucketDescription& bucket_desc)
    : geometry(bucket_desc.geometry),
      vertexBuffer(vertexBuffer),
      vertex_start(vertexBuffer.index()) {
}

void IconBucket::addFeature(const VectorTileFeature &feature, SpriteAtlas &sprite_atlas) {
    std::string field;

    if (geometry.field.size()) {
        auto field_it = feature.properties.find(geometry.field);
        if (field_it == feature.properties.end()) {
            fprintf(stderr, "feature doesn't contain field '%s'\n", geometry.field.c_str());
            return;
        }

        field = toString(field_it->second);
    } else {
        field = "<circle>";
    }

    const Rect<uint16_t> rect = sprite_atlas.getIcon(geometry.size, field);
    const uint16_t tx = rect.x + rect.w / 2;
    const uint16_t ty = rect.y + rect.h / 2;

    Geometry::command cmd;
    pbf geom = feature.geometry;
    Geometry geometry(geom);
    int32_t x, y;
    while ((cmd = geometry.next(x, y)) != Geometry::end) {
        if (cmd == Geometry::move_to) {
            vertexBuffer.add(x, y, tx, ty);
        } else {
            fprintf(stderr, "other command than move_to in icon geometry\n");
        }
    }

    vertex_end = vertexBuffer.index();
}

void IconBucket::render(Painter& painter, const std::string& layer_name, const Tile::ID& id) {
    painter.renderIcon(*this, layer_name, id);
}

bool IconBucket::hasData() const {
    return vertex_end > 0;
}

void IconBucket::drawIcons(IconShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer.itemSize);
    array.bind(shader, vertexBuffer, vertex_index);
    glDrawArrays(GL_POINTS, 0, (GLsizei)(vertex_end - vertex_start));
}

void IconBucket::drawIcons(DotShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer.itemSize);
    array.bind(shader, vertexBuffer, vertex_index);
    glDrawArrays(GL_POINTS, 0, (GLsizei)(vertex_end - vertex_start));
}
