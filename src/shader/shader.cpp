#include <llmr/shader/shader.hpp>
#include <llmr/platform/gl.hpp>
#if defined(DEBUG)
#include <llmr/util/timer.hpp>
#endif
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace llmr;

Shader::Shader(const GLchar *vertSource, const GLchar *fragSource)
    : valid(false),
      program(0) {
#if defined(DEBUG)
    util::timer timer("shader compilation");
#endif

    GLuint vertShader;
    if (!compileShader(&vertShader, GL_VERTEX_SHADER, vertSource)) {
#if defined(DEBUG)
        fprintf(stderr, "Vertex shader failed to compile: %s\n", vertSource);
#endif
        return;
    }

    GLuint fragShader;
    if (!compileShader(&fragShader, GL_FRAGMENT_SHADER, fragSource)) {
#if defined(DEBUG)
        fprintf(stderr, "Fragment shader failed to compile: %s\n", fragSource);
#endif
        return;
    }

    program = glCreateProgram();

    // Attach shaders
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);


    {
        // Link program
        GLint status;
        glLinkProgram(program);

#if defined(DEBUG)
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(program, logLength, &logLength, log);
            fprintf(stderr, "Program link log:\n%s", log);
            free(log);
        }
#endif

        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status == 0) {
            fprintf(stderr, "Program failed to link\n");
            glDeleteShader(vertShader);
            vertShader = 0;
            glDeleteShader(fragShader);
            fragShader = 0;
            glDeleteProgram(program);
            program = 0;
            return;
        }
    }

    {
        // Validate program
        GLint status;
        glValidateProgram(program);

#if defined(DEBUG)
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(program, logLength, &logLength, log);
            fprintf(stderr, "Program validate log:\n%s", log);
            free(log);
        }
#endif

        glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
        if (status == 0) {
#if defined(DEBUG)
            fprintf(stderr, "Program failed to validate\n");
#endif
            glDeleteShader(vertShader);
            vertShader = 0;
            glDeleteShader(fragShader);
            fragShader = 0;
            glDeleteProgram(program);
            program = 0;
        }

    }

    // Remove the compiled shaders; they are now part of the program.
    glDetachShader(program, vertShader);
    glDeleteShader(vertShader);
    glDetachShader(program, fragShader);
    glDeleteShader(fragShader);

    valid = true;
}


bool Shader::compileShader(GLuint *shader, GLenum type, const GLchar *source) {
    GLint status;

    *shader = glCreateShader(type);
    const GLchar *strings[] = { source };
    const GLint lengths[] = { (GLint)strlen(source) };
    glShaderSource(*shader, 1, strings, lengths);

    glCompileShader(*shader);

#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        fprintf(stderr, "Shader compile log:\n%s", log);
        free(log);
    }
#endif

    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(*shader);
        *shader = 0;
        return false;
    }

    return true;
}


void Shader::setMatrix(const std::array<float, 16>& newMatrix) {
    if (matrix != newMatrix) {
        glUniformMatrix4fv(u_matrix, 1, GL_FALSE, newMatrix.data());
        matrix = newMatrix;
    }
}


Shader::~Shader() {
    if (program) {
        glDeleteProgram(program);
        program = 0;
        valid = false;
    }
}
