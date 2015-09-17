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
static unsigned int current_used_bytes = 0;
static GLint current_bound_texture = 0;
static std::map<GLint, unsigned int> binding_to_size_map;

static GLuint current_array_buffer = 0;
static GLuint current_element_array_buffer = 0;
static std::map<GLint, GLsizeiptr> buffer_binding_to_size_map;
static unsigned int current_used_buffer_bytes = 0;
static unsigned int largest_amount_used_so_far = 0;

static std::mutex g_debug_mutex;

void mbx_glBindBuffer(GLenum target,
                      GLuint buffer) {
    g_debug_mutex.lock();
    if(target == GL_ARRAY_BUFFER) {
        current_array_buffer = buffer;
    } else if(target == GL_ELEMENT_ARRAY_BUFFER) {
        current_element_array_buffer = buffer;
    }
    g_debug_mutex.unlock();
    glBindBuffer(target, buffer);
}

void mbx_glDeleteBuffers(GLsizei n,
                     const GLuint * buffers) {
    g_debug_mutex.lock();
    for(int i = 0; i < n; ++i) {
        if(buffer_binding_to_size_map.find(buffers[i]) != buffer_binding_to_size_map.end()) {
            current_used_buffer_bytes -= buffer_binding_to_size_map[buffers[i]];
            std::cout << "GL glDeleteBuffers: " << buffers[i] << " freeing " << buffer_binding_to_size_map[buffers[i]] << " bytes current total " << current_used_buffer_bytes << "\n";
            buffer_binding_to_size_map.erase(buffers[i]);
        }
    }
    g_debug_mutex.unlock();
    glDeleteBuffers(n, buffers);
}

void mbx_glBufferData(GLenum target,
                      GLsizeiptr size,
                      const GLvoid * data,
                      GLenum usage) {
    g_debug_mutex.lock();
    GLuint current_binding = 0;
    if(target == GL_ARRAY_BUFFER) {
        current_binding = current_array_buffer;
    } else if(target == GL_ELEMENT_ARRAY_BUFFER) {
        current_binding = current_element_array_buffer;
    }
    if(buffer_binding_to_size_map.find(current_binding) != buffer_binding_to_size_map.end()) {
        current_used_buffer_bytes -= buffer_binding_to_size_map[current_binding];
        std::cout << "GL glBufferData: " << current_binding << " freeing " << buffer_binding_to_size_map[current_binding] << " bytes current total " << current_used_buffer_bytes << "\n";
    }
    buffer_binding_to_size_map[current_binding] = size;
    current_used_buffer_bytes += size;
    if (current_used_buffer_bytes > largest_amount_used_so_far) {
        largest_amount_used_so_far = current_used_buffer_bytes;
    }
    std::cout << "GL glBufferData: " << current_binding << " using " << buffer_binding_to_size_map[current_binding] << " bytes current total " << current_used_buffer_bytes << " high water mark " << largest_amount_used_so_far << "\n";
    g_debug_mutex.unlock();

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
    g_debug_mutex.lock();
    if (target == GL_TEXTURE_2D) {
        current_bound_texture = texture;
    }
    g_debug_mutex.unlock();
    glBindTexture(target, texture);
}

void mbx_glDeleteTextures(GLsizei n,
                          const GLuint * textures) {
    g_debug_mutex.lock();
    for(int i = 0; i < n; ++i) {
        if(binding_to_size_map.find(textures[i]) != binding_to_size_map.end()) {
            std::cout << "GL deleteTexture:" << textures[i] << "freeing " << binding_to_size_map[textures[i]] << " bytes current total " << current_used_bytes << "\n";
            current_used_bytes -= binding_to_size_map[textures[i]];
            binding_to_size_map.erase(textures[i]);
        }
    }
    g_debug_mutex.unlock();
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
    g_debug_mutex.lock();
    if (internalformat == GL_RGBA &&
        type == GL_UNSIGNED_BYTE) {
        if (binding_to_size_map.find(current_bound_texture) != binding_to_size_map.end()) {
            current_used_bytes -= binding_to_size_map[current_bound_texture];
            std::cout << "GL glTexImage2D: " << current_bound_texture << " freeing " << binding_to_size_map[current_bound_texture] << " bytes current total " << current_used_bytes << "\n";
        }
        binding_to_size_map[current_bound_texture] = width * height * 4;
        current_used_bytes += binding_to_size_map[current_bound_texture];
        std::cout << "GL glTexImage2D: " << current_bound_texture << " freeing " << binding_to_size_map[current_bound_texture] << " bytes current total " << current_used_bytes << "\n";
    }
    g_debug_mutex.unlock();
    glTexImage2D(target, level, internalformat, width, height, border, format, type, data);
}
#endif

