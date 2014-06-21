#include <llmr/platform/gl.hpp>

#include <iostream>

#ifdef NVIDIA
PFNGLDISCARDFRAMEBUFFEREXTPROC glDiscardFramebufferEXT;
#endif

void _CHECK_GL_ERROR(const char *cmd, const char *file, int line) {
    std::cout <<  cmd << ";" << std::endl;

    GLenum err;

    while ((err = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (err) {
            case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
            case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
            case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
        }

        std::cerr << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
        exit(1);
    }
}