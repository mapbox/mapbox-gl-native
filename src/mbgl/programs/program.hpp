#pragma once

#include <mbgl/gl/program.hpp>
#include <mbgl/programs/program_parameters.hpp>

#include <sstream>
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
        std::ostringstream pixelRatioSS;
        pixelRatioSS.imbue(std::locale("C"));
        pixelRatioSS.setf(std::ios_base::showpoint);
        pixelRatioSS << parameters.pixelRatio;
        return std::string("#define DEVICE_PIXEL_RATIO ") + pixelRatioSS.str() + "\n";
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
