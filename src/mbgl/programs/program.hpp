#pragma once

#include <mbgl/gl/program.hpp>
#include <mbgl/programs/program_parameters.hpp>

#include <cassert>

namespace mbgl {

template <class Shaders, class Primitive, class Attributes, class Uniforms>
class Program : public gl::Program<Primitive, Attributes, Uniforms> {
public:
    using ParentType = gl::Program<Primitive, Attributes, Uniforms>;

    Program(gl::Context& context, const ProgramParameters& programParameters)
        : ParentType(context, vertexSource(programParameters), fragmentSource(programParameters))
        {}
    
    static std::string pixelRatioDefine(const ProgramParameters& parameters) {
        return std::string("#define DEVICE_PIXEL_RATIO ") + std::to_string(parameters.pixelRatio) + "\n";
    }

    static std::string fragmentSource(const ProgramParameters& parameters) {
        std::string source = pixelRatioDefine(parameters) + Shaders::fragmentSource;
        if (parameters.overdraw) {
            assert(source.find("#ifdef OVERDRAW_INSPECTOR") != std::string::npos);
            source.replace(source.find_first_of('\n'), 1, "\n#define OVERDRAW_INSPECTOR\n");
        }
        return source;
    }

    static std::string vertexSource(const ProgramParameters& parameters) {
        return pixelRatioDefine(parameters) + Shaders::vertexSource;
    }

};

} // namespace mbgl
