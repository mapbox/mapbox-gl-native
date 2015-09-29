#include <mbgl/platform/gl.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/platform/log.hpp>

#include <cassert>
#include <iostream>
#include <map>
#include <mutex>

namespace mbgl {
namespace gl {

std::vector<ExtensionFunctionBase*>& ExtensionFunctionBase::functions() {
    static std::vector<ExtensionFunctionBase*> functions;
    return functions;
}

static std::once_flag initializeExtensionsOnce;

void InitializeExtensions(glProc (*getProcAddress)(const char *)) {
    std::call_once(initializeExtensionsOnce, [getProcAddress] {
        const char * extensionsPtr = reinterpret_cast<const char *>(
            MBGL_CHECK_ERROR(glGetString(GL_EXTENSIONS)));

        if (!extensionsPtr)
            return;

        const std::string extensions = extensionsPtr;
        for (auto fn : ExtensionFunctionBase::functions()) {
            for (auto probe : fn->probes) {
                if (extensions.find(probe.first) != std::string::npos) {
                    fn->ptr = getProcAddress(probe.second);
                    break;
                }
            }
        }
    });
}

void checkError(const char *cmd, const char *file, int line) {
    const GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        const char *error = nullptr;
        switch (err) {
            case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
            case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
#ifdef GL_STACK_UNDERFLOW
            case GL_STACK_UNDERFLOW:  error = "STACK_UNDERFLOW";  break;
#endif
#ifdef GL_STACK_OVERFLOW
            case GL_STACK_OVERFLOW:  error = "STACK_OVERFLOW";  break;
#endif
            default: error = "(unknown)"; break;
        }

        throw ::mbgl::gl::Error(err, std::string(cmd) + ": Error GL_" + error + " - " + file + ":" + util::toString(line));
    }
}
}
}

#ifdef GL_TRACK
#undef glBindTexture
#undef glDeleteTextures
#undef glTexImage2D
#undef glClear
#undef glShaderSource
#undef glBufferData
#undef glBindBuffer
#undef glDeleteBuffers
#undef glBufferData
static unsigned int currentUsedBytes = 0;
static GLint currentBoundTexture = 0;
static std::map<GLint, unsigned int> bindingToSizeMap;

static GLuint currentArrayBuffer = 0;
static GLuint currentElementArrayBuffer = 0;
static std::map<GLint, GLsizeiptr> bufferBindingToSizeMap;
static unsigned int currentUsedBufferBytes = 0;
static unsigned int largestAmountUsedSoFar = 0;

static std::mutex gDebugMutex;

void mbx_glBindBuffer(GLenum target,
                      GLuint buffer) {
    std::unique_lock<std::mutex> lock(gDebugMutex);
    if (target == GL_ARRAY_BUFFER) {
        currentArrayBuffer = buffer;
    } else if (target == GL_ELEMENT_ARRAY_BUFFER) {
        currentElementArrayBuffer = buffer;
    }
    lock.unlock();
    glBindBuffer(target, buffer);
}

void mbx_glDeleteBuffers(GLsizei n,
                     const GLuint * buffers) {
    std::unique_lock<std::mutex> lock(gDebugMutex);
    for (int i = 0; i < n; ++i) {
        if (bufferBindingToSizeMap.find(buffers[i]) != bufferBindingToSizeMap.end()) {
            currentUsedBufferBytes -= bufferBindingToSizeMap[buffers[i]];
            std::cout << "GL glDeleteBuffers: " << buffers[i] << " freeing " << bufferBindingToSizeMap[buffers[i]] << " bytes current total " << currentUsedBufferBytes << "\n";
            bufferBindingToSizeMap.erase(buffers[i]);
        }
    }
    lock.unlock();
    glDeleteBuffers(n, buffers);
}

void mbx_glBufferData(GLenum target,
                      GLsizeiptr size,
                      const GLvoid * data,
                      GLenum usage) {
    std::unique_lock<std::mutex> lock(gDebugMutex);
    GLuint currentBinding = 0;
    if (target == GL_ARRAY_BUFFER) {
        currentBinding = currentArrayBuffer;
    } else if (target == GL_ELEMENT_ARRAY_BUFFER) {
        currentBinding = currentElementArrayBuffer;
    }
    if (bufferBindingToSizeMap.find(currentBinding) != bufferBindingToSizeMap.end()) {
        currentUsedBufferBytes -= bufferBindingToSizeMap[currentBinding];
        std::cout << "GL glBufferData: " << currentBinding << " freeing " << bufferBindingToSizeMap[currentBinding] << " bytes current total " << currentUsedBufferBytes << "\n";
    }
    bufferBindingToSizeMap[currentBinding] = size;
    currentUsedBufferBytes += size;
    if (currentUsedBufferBytes > largestAmountUsedSoFar) {
        largestAmountUsedSoFar = currentUsedBufferBytes;
    }
    std::cout << "GL glBufferData: " << currentBinding << " using " << bufferBindingToSizeMap[currentBinding] << " bytes current total " << currentUsedBufferBytes << " high water mark " << largestAmountUsedSoFar << "\n";
    lock.unlock();
    
    glBufferData(target, size, data, usage);
}


void mbx_glShaderSource(GLuint shader,
                    GLsizei count,
                    const GLchar * const *string,
                        const GLint *length) {
    //std::cout << "Calling glShaderSource: " << *string << std::endl;
    glShaderSource(shader, count, string, length);
}

void mbx_glClear(GLbitfield mask) {
    //std::cout << "Calling glClear" << std::endl;
    glClear(mask);
}

void mbx_glBindTexture(	GLenum target,
                       GLuint texture) {
    std::unique_lock<std::mutex> lock(gDebugMutex);
    if (target == GL_TEXTURE_2D) {
        currentBoundTexture = texture;
    }
    lock.unlock();
    glBindTexture(target, texture);
}

void mbx_glDeleteTextures(GLsizei n,
                          const GLuint * textures) {
    std::unique_lock<std::mutex> lock(gDebugMutex);
    for (int i = 0; i < n; ++i) {
        if (bindingToSizeMap.find(textures[i]) != bindingToSizeMap.end()) {
            std::cout << "GL deleteTexture:" << textures[i] << "freeing " << bindingToSizeMap[textures[i]] << " bytes current total " << currentUsedBytes << "\n";
            currentUsedBytes -= bindingToSizeMap[textures[i]];
            bindingToSizeMap.erase(textures[i]);
        }
    }
    lock.unlock();
    glDeleteTextures(n, textures);
}

void mbx_glTexImage2D(GLenum target,
                      GLint level,
                      GLint internalformat,
                      GLsizei width,
                      GLsizei height,
                      GLint border,
                      GLenum format,
                      GLenum type,
                      const GLvoid * data) {
    std::unique_lock<std::mutex> lock(gDebugMutex);
    if (internalformat == GL_RGBA &&
        type == GL_UNSIGNED_BYTE) {
        if (bindingToSizeMap.find(currentBoundTexture) != bindingToSizeMap.end()) {
            currentUsedBytes -= bindingToSizeMap[currentBoundTexture];
            std::cout << "GL glTexImage2D: " << currentBoundTexture << " freeing " << bindingToSizeMap[currentBoundTexture] << " bytes current total " << currentUsedBytes << "\n";
        }
        bindingToSizeMap[currentBoundTexture] = width * height * 4;
        currentUsedBytes += bindingToSizeMap[currentBoundTexture];
        std::cout << "GL glTexImage2D: " << currentBoundTexture << " freeing " << bindingToSizeMap[currentBoundTexture] << " bytes current total " << currentUsedBytes << "\n";
    }
    lock.unlock();
    glTexImage2D(target, level, internalformat, width, height, border, format, type, data);
}
#endif

