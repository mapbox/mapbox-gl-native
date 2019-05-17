#pragma once

#include <mbgl/gfx/program.hpp>
#include <mbgl/gl/types.hpp>
#include <mbgl/gl/object.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/draw_scope_resource.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/gfx/uniform.hpp>
#include <mbgl/gl/vertex_array.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/util/io.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/programs/gl/shaders.hpp>

#include <string>

namespace mbgl {
namespace gl {

template <class Name>
class Program final : public gfx::Program<Name> {
public:
    using AttributeList = typename Name::AttributeList;
    using UniformList = typename Name::UniformList;
    using TextureList = typename Name::TextureList;

    Program(ProgramParameters programParameters_)
        : programParameters(std::move(programParameters_)) {
    }

    const ProgramParameters programParameters;

    static constexpr const auto vertexOffset = programs::gl::ShaderSource<Name>::vertexOffset;
    static constexpr const auto fragmentOffset = programs::gl::ShaderSource<Name>::fragmentOffset;

    class Instance {
    public:
        Instance(Context& context,
                 const std::initializer_list<const char*>& vertexSource,
                 const std::initializer_list<const char*>& fragmentSource)
            : program(context.createProgram(
                  context.createShader(ShaderType::Vertex, vertexSource),
                  context.createShader(ShaderType::Fragment, fragmentSource),
                  attributeLocations.getFirstAttribName())) {
            attributeLocations.queryLocations(program);
            uniformStates.queryLocations(program);
            // Texture units are specified via uniforms as well, so we need query their locations
            textureStates.queryLocations(program);
        }

        static std::unique_ptr<Instance>
        createInstance(gl::Context& context,
                       const ProgramParameters& programParameters,
                       const std::string& additionalDefines) {
            // Compile the shader
            const std::initializer_list<const char*> vertexSource = {
                programParameters.getDefines().c_str(),
                additionalDefines.c_str(),
                (programs::gl::shaderSource() + programs::gl::vertexPreludeOffset),
                (programs::gl::shaderSource() + vertexOffset)
            };
            const std::initializer_list<const char*> fragmentSource = {
                programParameters.getDefines().c_str(),
                additionalDefines.c_str(),
                (programs::gl::shaderSource() + programs::gl::fragmentPreludeOffset),
                (programs::gl::shaderSource() + fragmentOffset)
            };
            auto result = std::make_unique<Instance>(context, vertexSource, fragmentSource);

            return std::move(result);
        }

        UniqueProgram program;
        gl::AttributeLocations<AttributeList> attributeLocations;
        gl::UniformStates<UniformList> uniformStates;
        gl::TextureStates<TextureList> textureStates;
    };

    void draw(gfx::Context& genericContext,
              gfx::RenderPass&,
              const gfx::DrawMode& drawMode,
              const gfx::DepthMode& depthMode,
              const gfx::StencilMode& stencilMode,
              const gfx::ColorMode& colorMode,
              const gfx::CullFaceMode& cullFaceMode,
              const gfx::UniformValues<UniformList>& uniformValues,
              gfx::DrawScope& drawScope,
              const gfx::AttributeBindings<AttributeList>& attributeBindings,
              const gfx::TextureBindings<TextureList>& textureBindings,
              const gfx::IndexBuffer& indexBuffer,
              std::size_t indexOffset,
              std::size_t indexLength) override {
        auto& context = static_cast<gl::Context&>(genericContext);

        context.setDepthMode(depthMode);
        context.setStencilMode(stencilMode);
        context.setColorMode(colorMode);
        context.setCullFaceMode(cullFaceMode);

        const uint32_t key = gl::AttributeKey<AttributeList>::compute(attributeBindings);
        auto it = instances.find(key);
        if (it == instances.end()) {
            it = instances
                     .emplace(key,
                              Instance::createInstance(
                                  context,
                                  programParameters,
                                  gl::AttributeKey<AttributeList>::defines(attributeBindings)))
                     .first;
        }

        auto& instance = *it->second;
        context.program = instance.program;

        instance.uniformStates.bind(uniformValues);

        instance.textureStates.bind(context, textureBindings);

        auto& vertexArray = drawScope.getResource<gl::DrawScopeResource>().vertexArray;
        vertexArray.bind(context,
                        indexBuffer,
                        instance.attributeLocations.toBindingArray(attributeBindings));

        context.draw(drawMode,
                     indexOffset,
                     indexLength);
    }

private:
    std::map<uint32_t, std::unique_ptr<Instance>> instances;
};

} // namespace gl
} // namespace mbgl
