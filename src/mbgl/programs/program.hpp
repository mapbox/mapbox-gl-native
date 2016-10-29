#pragma once

#include <mbgl/gl/program.hpp>

#include <cassert>

namespace mbgl {

enum class ProgramDefines : bool {
    None = false,
    Overdraw = true,
};

template <class Shaders, class As, class Us>
class Program : public gl::Program<As, Us> {
public:
    Program(gl::Context& context, ProgramDefines defines)
        : gl::Program<As, Us>(context, Shaders::vertexSource, fragmentSource(defines))
        {}

    static std::string fragmentSource(ProgramDefines defines) {
        std::string fragment = Shaders::fragmentSource;
        if (defines == ProgramDefines::Overdraw) {
            assert(fragment.find("#ifdef OVERDRAW_INSPECTOR") != std::string::npos);
            fragment.replace(fragment.find_first_of('\n'), 1, "\n#define OVERDRAW_INSPECTOR\n");
        }
        return fragment;
    }
};

} // namespace mbgl
