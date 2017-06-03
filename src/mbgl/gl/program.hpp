#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/gl/object.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>

#include <mbgl/util/io.hpp>
#include <mbgl/programs/binary_program.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/shaders/shaders.hpp>


#include <string>

namespace mbgl {
namespace gl {

template <class P, class As, class Us>
class Program {
public:
    using Primitive = P;
    using Attributes = As;
    using Uniforms = Us;

    using UniformValues = typename Uniforms::Values;
    using AttributeBindings = typename Attributes::Bindings;

    Program(Context& context, const std::string& vertexSource, const std::string& fragmentSource)
        : program(
              context.createProgram(context.createShader(ShaderType::Vertex, vertexSource),
                                    context.createShader(ShaderType::Fragment, fragmentSource))),
          attributeLocations(Attributes::bindLocations(program)),
          uniformsState((context.linkProgram(program), Uniforms::bindLocations(program))) {
    }

    template <class BinaryProgram>
    Program(Context& context, const BinaryProgram& binaryProgram)
        : program(context.createProgram(binaryProgram.format(), binaryProgram.code())),
          attributeLocations(Attributes::loadNamedLocations(binaryProgram)),
          uniformsState(Uniforms::loadNamedLocations(binaryProgram)) {
    }
    
    static Program createProgram(gl::Context& context,
                                 const ProgramParameters& programParameters,
                                 const char* name,
                                 const char* vertexSource_,
                                 const char* fragmentSource_) {
#if MBGL_HAS_BINARY_PROGRAMS
        if (!programParameters.cacheDir.empty() && context.supportsProgramBinaries()) {
            const std::string vertexSource =
                shaders::vertexSource(programParameters, vertexSource_);
            const std::string fragmentSource =
                shaders::fragmentSource(programParameters, fragmentSource_);
            const std::string cachePath =
                shaders::programCachePath(programParameters, name);
            const std::string identifier =
                shaders::programIdentifier(vertexSource, fragmentSource_);

            try {
                if (auto cachedBinaryProgram = util::readFile(cachePath)) {
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
                    util::write_file(cachePath, binaryProgram->serialize());
                    Log::Warning(Event::OpenGL, "Caching program in: %s", cachePath.c_str());
                }
            } catch (std::runtime_error& error) {
                Log::Warning(Event::OpenGL, "Failed to cache program: %s", error.what());
            }

            return std::move(result);
        }
#endif
        (void)name;
        return Program {
            context, shaders::vertexSource(programParameters, vertexSource_),
            shaders::fragmentSource(programParameters, fragmentSource_)
        };
    }

    template <class BinaryProgram>
    optional<BinaryProgram> get(Context& context, const std::string& identifier) const {
        if (auto binaryProgram = context.getBinaryProgram(program)) {
            return BinaryProgram{ binaryProgram->first, std::move(binaryProgram->second),
                                  identifier, Attributes::getNamedLocations(attributeLocations),
                                  Uniforms::getNamedLocations(uniformsState) };
        }
        return {};
    }

    template <class DrawMode>
    void draw(Context& context,
              DrawMode drawMode,
              DepthMode depthMode,
              StencilMode stencilMode,
              ColorMode colorMode,
              UniformValues&& uniformValues,
              AttributeBindings&& attributeBindings,
              const IndexBuffer<DrawMode>& indexBuffer,
              const SegmentVector& segments) {
        static_assert(std::is_same<Primitive, typename DrawMode::Primitive>::value, "incompatible draw mode");

        context.setDrawMode(drawMode);
        context.setDepthMode(depthMode);
        context.setStencilMode(stencilMode);
        context.setColorMode(colorMode);

        context.program = program;

        Uniforms::bind(uniformsState, std::move(uniformValues));

        for (const auto& segment : segments) {
            // TODO: (Re-add VAO support) No VAO support. Force attributes to be rebound.
            context.elementBuffer = indexBuffer.buffer;
            variableBindings = {};

            Attributes::bind(context,
                         attributeLocations,
                         variableBindings,
                         attributeBindings,
                         segment.vertexOffset);


            context.draw(drawMode.primitiveType,
                         segment.indexOffset,
                         segment.indexLength);
        }
    }

private:
    UniqueProgram program;

    typename Attributes::Locations attributeLocations;
    typename Attributes::VariableBindings variableBindings;

    typename Uniforms::State uniformsState;
};

} // namespace gl
} // namespace mbgl
