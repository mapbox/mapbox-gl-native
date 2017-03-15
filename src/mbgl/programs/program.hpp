#pragma once

#include <mbgl/gl/program.hpp>
#include <mbgl/gl/features.hpp>
#include <mbgl/programs/binary_program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/shaders/shaders.hpp>
#include <mbgl/util/io.hpp>

namespace mbgl {

template <class Shaders,
          class Primitive,
          class LayoutAttrs,
          class Uniforms,
          class PaintProperties>
class Program {
public:
    using LayoutAttributes = LayoutAttrs;
    using LayoutVertex = typename LayoutAttributes::Vertex;

    using PaintPropertyBinders = typename PaintProperties::Binders;
    using PaintAttributes = typename PaintPropertyBinders::Attributes;
    using Attributes = gl::ConcatenateAttributes<LayoutAttributes, PaintAttributes>;

    using UniformValues = typename Uniforms::Values;
    using PaintUniforms = typename PaintPropertyBinders::Uniforms;
    using AllUniforms = gl::ConcatenateUniforms<Uniforms, PaintUniforms>;

    using ProgramType = gl::Program<Primitive, Attributes, AllUniforms>;

    ProgramType program;

    Program(gl::Context& context, const ProgramParameters& programParameters)
        : program([&] {
#if MBGL_HAS_BINARY_PROGRAMS
              if (!programParameters.cacheDir.empty() && context.supportsProgramBinaries()) {
                  const std::string vertexSource =
                      shaders::vertexSource(programParameters, Shaders::vertexSource);
                  const std::string fragmentSource =
                      shaders::fragmentSource(programParameters, Shaders::fragmentSource);
                  const std::string cachePath =
                      shaders::programCachePath(programParameters, Shaders::name);
                  const std::string identifier =
                      shaders::programIdentifier(vertexSource, fragmentSource);

                  try {
                      if (auto cachedBinaryProgram = util::readFile(cachePath)) {
                          const BinaryProgram binaryProgram(std::move(*cachedBinaryProgram));
                          if (binaryProgram.identifier() == identifier) {
                              return ProgramType{ context, binaryProgram };
                          } else {
                              Log::Warning(Event::OpenGL,
                                           "Cached program %s changed. Recompilation required.",
                                           Shaders::name);
                          }
                      }
                  } catch (std::runtime_error& error) {
                      Log::Warning(Event::OpenGL, "Could not load cached program: %s",
                                   error.what());
                  }

                  // Compile the shader
                  ProgramType result{ context, vertexSource, fragmentSource };

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
              return ProgramType{
                  context, shaders::vertexSource(programParameters, Shaders::vertexSource),
                  shaders::fragmentSource(programParameters, Shaders::fragmentSource)
              };
          }()) {
    }

    template <class DrawMode>
    void draw(gl::Context& context,
              DrawMode drawMode,
              gl::DepthMode depthMode,
              gl::StencilMode stencilMode,
              gl::ColorMode colorMode,
              UniformValues&& uniformValues,
              const gl::VertexBuffer<LayoutVertex>& layoutVertexBuffer,
              const gl::IndexBuffer<DrawMode>& indexBuffer,
              const gl::SegmentVector<Attributes>& segments,
              const PaintPropertyBinders& paintPropertyBinders,
              const typename PaintProperties::Evaluated& currentProperties,
              float currentZoom) {
        program.draw(
            context,
            std::move(drawMode),
            std::move(depthMode),
            std::move(stencilMode),
            std::move(colorMode),
            uniformValues
                .concat(paintPropertyBinders.uniformValues(currentZoom)),
            LayoutAttributes::allVariableBindings(layoutVertexBuffer)
                .concat(paintPropertyBinders.attributeBindings(currentProperties)),
            indexBuffer,
            segments
        );
    }
};

} // namespace mbgl
