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
#include <mbgl/gl/features.hpp>
#include <mbgl/gl/binary_program.hpp>
#include <mbgl/util/io.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/programs/program_parameters.hpp>
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
  
    Program(Context& context, const std::string& vertexSource, const std::string& fragmentSource)
        : program(
              context.createProgram(context.createShader(ShaderType::Vertex, vertexSource),
                                    context.createShader(ShaderType::Fragment, fragmentSource))),
          attributeLocations(context, program) {
        // Re-link program after manually binding only active attributes in Attributes::queryLocations
        context.linkProgram(program);

        // We have to re-initialize the uniforms state from the bindings as the uniform locations
        // get shifted on some implementations
        uniformStates.queryLocations(program);

        // Texture units are specified via uniforms as well, so we need query their locations
        textureStates.queryLocations(program);
    }

    template <class BinaryProgram>
    Program(Context& context, const BinaryProgram& binaryProgram)
        : program(context.createProgram(binaryProgram.format(), binaryProgram.code())),
          attributeLocations(binaryProgram) {
        uniformStates.loadNamedLocations(binaryProgram);
        textureStates.loadNamedLocations(binaryProgram);
    }

    static std::unique_ptr<Program> createProgram(gl::Context& context,
                                                  const ProgramParameters& programParameters,
                                                  const char* name,
                                                  const char* vertexSource_,
                                                  const char* fragmentSource_) {
        const std::string vertexSource = programs::gl::vertexSource(programParameters, vertexSource_);
        const std::string fragmentSource = programs::gl::fragmentSource(programParameters, fragmentSource_);

#if MBGL_HAS_BINARY_PROGRAMS
        optional<std::string> cachePath = programParameters.cachePath(name);
        if (cachePath && context.supportsProgramBinaries()) {
            const std::string identifier = programs::gl::programIdentifier(vertexSource, fragmentSource);

            try {
                if (auto cachedBinaryProgram = util::readFile(*cachePath)) {
                    const BinaryProgram binaryProgram(std::move(*cachedBinaryProgram));
                    if (binaryProgram.identifier() == identifier) {
                        return std::make_unique<Program>(context, binaryProgram);
                    } else {
                        Log::Warning(Event::OpenGL,
                                     "Cached program %s changed. Recompilation required.",
                                     name);
                    }
                }
            } catch (std::runtime_error& error) {
                Log::Warning(Event::OpenGL, "Could not load cached program: %s",
                             error.what());
            }

            // Compile the shader
            auto result = std::make_unique<Program>(context, vertexSource, fragmentSource);

            try {
                if (const auto binaryProgram =
                        result->template get<BinaryProgram>(context, identifier)) {
                    util::write_file(*cachePath, binaryProgram->serialize());
                    Log::Warning(Event::OpenGL, "Caching program in: %s", (*cachePath).c_str());
                }
            } catch (std::runtime_error& error) {
                Log::Warning(Event::OpenGL, "Failed to cache program: %s", error.what());
            }

            return std::move(result);
        }
#endif

        (void)name;
        return std::make_unique<Program>(context, vertexSource, fragmentSource);
    }

    template <class BinaryProgram>
    optional<BinaryProgram> get(Context& context, const std::string& identifier) const {
        if (auto binaryProgram = context.getBinaryProgram(program)) {
            return BinaryProgram{ binaryProgram->first,
                                  std::move(binaryProgram->second),
                                  identifier,
                                  attributeLocations.getNamedLocations(),
                                  uniformStates.getNamedLocations(),
                                  textureStates.getNamedLocations() };
        }
        return {};
    }

    void draw(gfx::Context& genericContext,
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
        auto& context = reinterpret_cast<gl::Context&>(genericContext);

        context.setDepthMode(depthMode);
        context.setStencilMode(stencilMode);
        context.setColorMode(colorMode);
        context.setCullFaceMode(cullFaceMode);

        context.program = program;

        uniformStates.bind(uniformValues);

        textureStates.bind(context, textureBindings);

        auto& vertexArray = reinterpret_cast<gl::DrawScopeResource&>(*drawScope.resource).vertexArray;
        vertexArray.bind(context,
                        indexBuffer,
                        attributeLocations.toBindingArray(attributeBindings));

        context.draw(drawMode,
                     indexOffset,
                     indexLength);
    }

private:
    UniqueProgram program;

    gl::AttributeLocations<AttributeList> attributeLocations;
    gl::UniformStates<UniformList> uniformStates;
    gl::TextureStates<TextureList> textureStates;
};

} // namespace gl
} // namespace mbgl
