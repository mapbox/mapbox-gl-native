#pragma once

#include <mbgl/gl/vao.hpp>
#include <mbgl/renderer/render_pass.hpp>

namespace mbgl {

template <class... Shaders>
struct ElementGroup {
    template <class Shader>
    struct VAOs {
        gl::VertexArrayObject normalVAO;
        gl::VertexArrayObject overdrawVAO;
    };

    std::tuple<VAOs<Shaders>...> vaos;

    template <class Shader>
    gl::VertexArrayObject& getVAO(const Shader&, PaintMode paintMode) {
        auto& vao = std::get<VAOs<Shader>>(vaos);
        return paintMode == PaintMode::Overdraw ? vao.overdrawVAO : vao.normalVAO;
    }

    std::size_t vertexLength = 0;
    std::size_t indexLength = 0;
};

} // namespace mbgl
