#pragma once

#include <mbgl/renderer/renderable.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/geometry/collision_box_buffer.hpp>

namespace mbgl {

class SymbolRenderable : public Renderable {
public:
    using TextElementGroup = ElementGroup<1>;
    using IconElementGroup = ElementGroup<2>;
    using CollisionBoxElementGroup = ElementGroup<1>;

    void upload(gl::ObjectStore& store) override {
        if (!uploaded) {
            if (!text.groups.empty()) {
                text.vertices.upload(store);
                text.triangles.upload(store);
            }
            if (!icon.groups.empty()) {
                icon.vertices.upload(store);
                icon.triangles.upload(store);
            }
            uploaded = true;
        }
    }

    struct TextBuffer {
        TextVertexBuffer vertices;
        TriangleElementsBuffer triangles;
        std::vector<std::unique_ptr<TextElementGroup>> groups;
    } text;

    struct IconBuffer {
        IconVertexBuffer vertices;
        TriangleElementsBuffer triangles;
        std::vector<std::unique_ptr<IconElementGroup>> groups;
    } icon;

    struct CollisionBoxBuffer {
        CollisionBoxVertexBuffer vertices;
        std::vector<std::unique_ptr<CollisionBoxElementGroup>> groups;
    } collisionBox;
};

} // namespace mbgl
