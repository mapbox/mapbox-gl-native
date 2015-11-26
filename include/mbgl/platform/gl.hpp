#ifndef MBGL_RENDERER_GL
#define MBGL_RENDERER_GL

//#define GL_TRACK

#include <string>
#include <stdexcept>
#include <vector>

#ifdef GL_TRACK
#include <iostream>
#endif

#if __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
    #elif TARGET_IPHONE_SIMULATOR
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
    #elif TARGET_OS_MAC
        #include <OpenGL/OpenGL.h>
        #include <OpenGL/gl.h>
    #else
        #error Unsupported Apple platform
    #endif
#elif __ANDROID__ || MBGL_USE_GLES2
    #define GL_GLEXT_PROTOTYPES
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#else
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif

namespace mbgl {
namespace gl {

#ifdef GL_TRACK
    typedef void (*GLDEBUGPROC)(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar *message,
                                const void *userParam);
    
    template <class... Args> void mbx_trapExtension(const char *name, Args... args);
    
    void mbx_trapExtension(const char *);
    void mbx_trapExtension(const char *, GLint, const char *);
    void mbx_trapExtension(const char *, GLsizei, GLuint *);
    void mbx_trapExtension(const char *, GLsizei, const GLuint *);
    void mbx_trapExtension(const char *, GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean);
    void mbx_trapExtension(const char *, GLenum, GLuint, GLsizei, const GLchar *);
    void mbx_trapExtension(const char *, GLDEBUGPROC, const void *);
    void mbx_trapExtension(const char *, GLuint, GLuint, GLuint, GLuint, GLint, const char *, const void*);
    void mbx_trapExtension(const char *name, GLuint array);
#endif
    
struct Error : ::std::runtime_error {
    inline Error(GLenum err, const std::string &msg) : ::std::runtime_error(msg), code(err) {};
    const GLenum code;
};

void checkError(const char *cmd, const char *file, int line);

#if defined(DEBUG)
#define MBGL_CHECK_ERROR(cmd) ([&]() { struct __MBGL_C_E { inline ~__MBGL_C_E() { ::mbgl::gl::checkError(#cmd, __FILE__, __LINE__); } } __MBGL_C_E; return cmd; }())
#else
#define MBGL_CHECK_ERROR(cmd) (cmd)
#endif

class ExtensionFunctionBase {
public:
    static std::vector<ExtensionFunctionBase*>& functions();
    typedef std::pair<const char *, const char *> Probe;
    std::vector<Probe> probes;
    void (*ptr)();
#ifdef GL_TRACK
    const char *foundName;
#endif
};

template <class>
class ExtensionFunction;

template <class R, class... Args>
class ExtensionFunction<R (Args...)> : protected ExtensionFunctionBase {
public:
    ExtensionFunction(std::initializer_list<Probe> probes_) {
        probes = probes_;
        ExtensionFunctionBase::functions().push_back(this);
    }

    explicit operator bool() const {
        return ptr;
    }

    R operator()(Args... args) const {
#ifdef GL_TRACK
        mbx_trapExtension(foundName, args...);
#endif
        return (*reinterpret_cast<R (*)(Args...)>(ptr))(std::forward<Args>(args)...);
    }
};

using glProc = void (*)();
void InitializeExtensions(glProc (*getProcAddress)(const char *));

}
}

#ifdef GL_ES_VERSION_2_0
    #define glClearDepth glClearDepthf
    #define glDepthRange glDepthRangef
#endif

#ifdef GL_TRACK
#define glBindTexture mbx_glBindTexture
#define glDeleteTexture mbx_glDeleteTexture
#define glTexImage2D mbx_glTexImage2D
#define glClear mbx_glClear
#define glShaderSource mbx_glShaderSource
#define glBufferData mbx_glBufferData
#define glBindBuffer mbx_glBindBuffer
#define glDeleteBuffers mbx_glDeleteBuffers

void mbx_glDeleteBuffers(GLsizei n,
                     const GLuint * buffers);
void mbx_glBindBuffer(GLenum target,
                      GLuint buffer);
void mbx_glBufferData(GLenum target,
                      GLsizeiptr size,
                      const GLvoid * data,
                      GLenum usage);
void mbx_glShaderSource(GLuint shader,
                        GLsizei count,
                        const GLchar * const *string,
                        const GLint *length);
void mbx_glClear(GLbitfield mask);
void mbx_glBindTexture(	GLenum target,
                       GLuint texture);
void mbx_glDeleteTextures(GLsizei n,
                          const GLuint * textures);
void mbx_glTexImage2D(GLenum target,
                      GLint level,
                      GLint internalformat,
                      GLsizei width,
                      GLsizei height,
                      GLint border,
                      GLenum format,
                      GLenum type,
                      const GLvoid * data);
#endif

#endif
