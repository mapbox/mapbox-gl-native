#include <mbgl/shader/shader.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/util/stopwatch.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>

#include <cstring>
#include <cassert>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>

namespace mbgl {

Shader::Shader(const char* name_, const char* vertexSource, const char* fragmentSource, gl::ObjectStore& store)
    : name(name_)
    , program(store.createProgram())
    , vertexShader(store.createShader(GL_VERTEX_SHADER))
    , fragmentShader(store.createShader(GL_FRAGMENT_SHADER))
{
    util::stopwatch stopwatch("shader compilation", Event::Shader);

    if (!compileShader(vertexShader, vertexSource)) {
        Log::Error(Event::Shader, "Vertex shader %s failed to compile: %s", name, vertexSource);
        throw util::ShaderException(std::string { "Vertex shader " } + name + " failed to compile");
    }

    if (!compileShader(fragmentShader, fragmentSource)) {
        Log::Error(Event::Shader, "Fragment shader %s failed to compile: %s", name, fragmentSource);
        throw util::ShaderException(std::string { "Fragment shader " } + name + " failed to compile");
    }

    // Attach shaders
    MBGL_CHECK_ERROR(glAttachShader(program.get(), vertexShader.get()));
    MBGL_CHECK_ERROR(glAttachShader(program.get(), fragmentShader.get()));

    linkProgram(program);

    std::string overdrawSource(fragmentSource);
    if (overdrawSource.find("#ifdef OVERDRAW_INSPECTOR") != std::string::npos) {
        programOverdraw = store.createProgram();
        overdrawShader = store.createShader(GL_FRAGMENT_SHADER);

        overdrawSource.replace(overdrawSource.find_first_of('\n'), 1, "\n#define OVERDRAW_INSPECTOR\n");
        if (!compileShader(*overdrawShader, overdrawSource.c_str())) {
            Log::Error(Event::Shader, "Overdraw shader %s failed to compile: %s", name, overdrawSource.c_str());
            throw util::ShaderException(std::string { "Overdraw shader " } + name + " failed to compile");
        }

        MBGL_CHECK_ERROR(glAttachShader(*programOverdraw, vertexShader.get()));
        MBGL_CHECK_ERROR(glAttachShader(*programOverdraw, *overdrawShader));
        linkProgram(*programOverdraw);
    }

    a_pos = MBGL_CHECK_ERROR(glGetAttribLocation(program.get(), "a_pos"));
}

void Shader::linkProgram(gl::UniqueProgram& program_) {
    // Link program
    GLint status;
     MBGL_CHECK_ERROR(glLinkProgram(program_.get()));

    MBGL_CHECK_ERROR(glGetProgramiv(program_.get(), GL_LINK_STATUS, &status));
    if (status == 0) {
        GLint logLength;
        MBGL_CHECK_ERROR(glGetProgramiv(program_.get(), GL_INFO_LOG_LENGTH, &logLength));
        const auto log = std::make_unique<GLchar[]>(logLength);
        if (logLength > 0) {
            MBGL_CHECK_ERROR(glGetProgramInfoLog(program_.get(), logLength, &logLength, log.get()));
            Log::Error(Event::Shader, "Program failed to link: %s", log.get());
        }
        throw util::ShaderException(std::string { "Program " } + name + " failed to link: " + log.get());
    }
}

bool Shader::compileShader(gl::UniqueShader& shader, const GLchar *source) {
    GLint status = 0;

    const GLsizei lengths = static_cast<GLsizei>(std::strlen(source));
    MBGL_CHECK_ERROR(glShaderSource(shader.get(), 1, &source, &lengths));

    MBGL_CHECK_ERROR(glCompileShader(shader.get()));

    MBGL_CHECK_ERROR(glGetShaderiv(shader.get(), GL_COMPILE_STATUS, &status));
    if (status == 0) {
        GLint logLength;
        MBGL_CHECK_ERROR(glGetShaderiv(shader.get(), GL_INFO_LOG_LENGTH, &logLength));
        if (logLength > 0) {
            const auto log = std::make_unique<GLchar[]>(logLength);
            MBGL_CHECK_ERROR(glGetShaderInfoLog(shader.get(), logLength, &logLength, log.get()));
            Log::Error(Event::Shader, "Shader failed to compile: %s", log.get());
        }
        return false;
    }

    MBGL_CHECK_ERROR(glGetShaderiv(shader.get(), GL_COMPILE_STATUS, &status));
    if (status == GL_FALSE) {
        Log::Error(Event::Shader, "Shader %s failed to compile.", name);
        return false;
    }

    return true;
}

Shader::~Shader() {
    if (program.get()) {
        MBGL_CHECK_ERROR(glDetachShader(program.get(), vertexShader.get()));
        MBGL_CHECK_ERROR(glDetachShader(program.get(), fragmentShader.get()));
    }
    if (programOverdraw) {
        MBGL_CHECK_ERROR(glDetachShader(*programOverdraw, vertexShader.get()));
        MBGL_CHECK_ERROR(glDetachShader(*programOverdraw, *overdrawShader));
    }
}

} // namespace mbgl
