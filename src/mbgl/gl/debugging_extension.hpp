#pragma once

#include <mbgl/gl/extension.hpp>
#include <mbgl/gl/gl.hpp>

#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION        0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM      0x8245
#define GL_DEBUG_SOURCE_API               0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM     0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER   0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY       0x8249
#define GL_DEBUG_SOURCE_APPLICATION       0x824A
#define GL_DEBUG_SOURCE_OTHER             0x824B
#define GL_DEBUG_TYPE_ERROR               0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  0x824E
#define GL_DEBUG_TYPE_PORTABILITY         0x824F
#define GL_DEBUG_TYPE_PERFORMANCE         0x8250
#define GL_DEBUG_TYPE_OTHER               0x8251
#define GL_MAX_DEBUG_MESSAGE_LENGTH       0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES      0x9144
#define GL_DEBUG_LOGGED_MESSAGES          0x9145
#define GL_DEBUG_SEVERITY_HIGH            0x9146
#define GL_DEBUG_SEVERITY_MEDIUM          0x9147
#define GL_DEBUG_SEVERITY_LOW             0x9148
#define GL_DEBUG_TYPE_MARKER              0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP          0x8269
#define GL_DEBUG_TYPE_POP_GROUP           0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION    0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH    0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH        0x826D
#define GL_BUFFER                         0x82E0
#define GL_SHADER                         0x82E1
#define GL_PROGRAM                        0x82E2
#define GL_QUERY                          0x82E3
#define GL_PROGRAM_PIPELINE               0x82E4
#define GL_SAMPLER                        0x82E6
#define GL_MAX_LABEL_LENGTH               0x82E8
#define GL_DEBUG_OUTPUT                   0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT         0x00000002
#define GL_DISPLAY_LIST                   0x82E7
#define GL_VERTEX_ARRAY                   0x8074
#define GL_TRANSFORM_FEEDBACK             0x8E22
#define GL_TEXTURE                        0x1702
#define GL_RENDERBUFFER                   0x8D41
#define GL_FRAMEBUFFER                    0x8D40

namespace mbgl {
namespace gl {
namespace extension {

class Debugging {
public:
    using Callback = void (*)(GLenum source,
                              GLenum type,
                              GLuint id,
                              GLenum severity,
                              GLsizei length,
                              const GLchar* message,
                              const void* userParam);

    static void DebugCallback(GLenum source,
                              GLenum type,
                              GLuint id,
                              GLenum severity,
                              GLsizei /* length */,
                              const GLchar* message,
                              const void* /* userParam */);

    template <typename Fn>
    Debugging(const Fn& loadExtension)
        :
#ifndef NDEBUG
          pushDebugGroup(
              loadExtension({ { "GL_KHR_debug", "glPushDebugGroup" } })),
          popDebugGroup(
              loadExtension({ { "GL_KHR_debug", "glPopDebugGroup" } })),
          pushGroupMarkerEXT(
              loadExtension({ { "GL_EXT_debug_marker", "glPushGroupMarkerEXT" } })),
          popGroupMarkerEXT(
              loadExtension({ { "GL_EXT_debug_marker", "glPopGroupMarkerEXT" } })),
#endif
          debugMessageControl(
              loadExtension({ { "GL_KHR_debug", "glDebugMessageControl" },
                              { "GL_ARB_debug_output", "glDebugMessageControlARB" } })),
          debugMessageCallback(
              loadExtension({ { "GL_KHR_debug", "glDebugMessageCallback" },
                              { "GL_ARB_debug_output", "glDebugMessageCallbackARB" } })) {
    }

#ifndef NDEBUG
    const ExtensionFunction<void(GLenum source,
                                 GLuint id,
                                 GLsizei length,
                                 const GLchar* message)> pushDebugGroup;

    const ExtensionFunction<void()> popDebugGroup;

    const ExtensionFunction<void(GLsizei length,
                                 const GLchar* marker)> pushGroupMarkerEXT;

    const ExtensionFunction<void()> popGroupMarkerEXT;
#endif

    const ExtensionFunction<void(GLenum source,
                                 GLenum type,
                                 GLenum severity,
                                 GLsizei count,
                                 const GLuint* ids,
                                 GLboolean enabled)> debugMessageControl;

    const ExtensionFunction<void(Callback callback,
                                 const void* userParam)> debugMessageCallback;
};

} // namespace extension
} // namespace gl
} // namespace mbgl
