#pragma once

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
#include <mbgl/shaders/shaders.hpp>

#include <string>

namespace mbgl {
namespace gl {

template <class P, class AttributeList, class UniformList, class TextureList>
class Program {
public:
    using Primitive = P;

    using AttributeBindings = gfx::AttributeBindings<AttributeList>;
    using UniformValues = gfx::UniformValues<UniformList>;
    using TextureBindings = gfx::TextureBindings<TextureList>;

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
        textures.queryLocations(program);
    }

    template <class BinaryProgram>
    Program(Context& context, const BinaryProgram& binaryProgram)
        : program(context.createProgram(binaryProgram.format(), binaryProgram.code())),
          attributeLocations(binaryProgram) {
        uniformStates.loadNamedLocations(binaryProgram);
        textures.loadNamedLocations(binaryProgram);
    }

    static Program createProgram(gl::Context& context,
                                 const ProgramParameters& programParameters,
                                 const char* name,
                                 const char* vertexSource_,
                                 const char* fragmentSource_) {
        const std::string vertexSource = shaders::vertexSource(programParameters, vertexSource_);
        const std::string fragmentSource = shaders::fragmentSource(programParameters, fragmentSource_);

#if MBGL_HAS_BINARY_PROGRAMS
        optional<std::string> cachePath = programParameters.cachePath(name);
        if (cachePath && context.supportsProgramBinaries()) {
            const std::string identifier = shaders::programIdentifier(vertexSource, fragmentSource);

            try {
                if (auto cachedBinaryProgram = util::readFile(*cachePath)) {
                    const BinaryProgram binaryProgram(std::move(*cachedBinaryProgram));
                    if (binaryProgram.identifier() == identifier) {
                        return Program { context, binaryProgram };
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
            Program result{ context, vertexSource, fragmentSource };

            try {
                if (const auto binaryProgram =
                        result.template get<BinaryProgram>(context, identifier)) {
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
        return Program { context, vertexSource, fragmentSource };
    }

    template <class BinaryProgram>
    optional<BinaryProgram> get(Context& context, const std::string& identifier) const {
        if (auto binaryProgram = context.getBinaryProgram(program)) {
            return BinaryProgram{ binaryProgram->first,
                                  std::move(binaryProgram->second),
                                  identifier,
                                  attributeLocations.getNamedLocations(),
                                  uniformStates.getNamedLocations(),
                                  textures.getNamedLocations() };
        }
        return {};
    }

    template <class DrawMode>
    void draw(gfx::Context& genericContext,
              DrawMode drawMode,
              gfx::DepthMode depthMode,
              gfx::StencilMode stencilMode,
              gfx::ColorMode colorMode,
              gfx::CullFaceMode cullFaceMode,
              const UniformValues& uniformValues,
              gfx::DrawScope& drawScope,
              const AttributeBindings& attributeBindings,
              const TextureBindings& textureBindings,
              const gfx::IndexBuffer& indexBuffer,
              std::size_t indexOffset,
              std::size_t indexLength) {
        static_assert(std::is_same<Primitive, typename DrawMode::Primitive>::value, "incompatible draw mode");
        auto& context = reinterpret_cast<gl::Context&>(genericContext);

        context.setDrawMode(drawMode);
        context.setDepthMode(depthMode);
        context.setStencilMode(stencilMode);
        context.setColorMode(colorMode);
        context.setCullFaceMode(cullFaceMode);

        context.program = program;

        uniformStates.bind(uniformValues);

        textures.bind(context, textureBindings);

        auto& vertexArray = reinterpret_cast<gl::DrawScopeResource&>(*drawScope.resource).vertexArray;
        vertexArray.bind(context,
                        indexBuffer,
                        attributeLocations.toBindingArray(attributeBindings));

        context.draw(drawMode.primitiveType,
                     indexOffset,
                     indexLength);
    }

private:
    UniqueProgram program;

    gl::UniformStates<UniformList> uniformStates;
    gl::AttributeLocations<AttributeList> attributeLocations;
    gl::Textures<TextureList> textures;
};

} // namespace gl
} // namespace mbgl
