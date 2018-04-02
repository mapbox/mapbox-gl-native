#include <jni.h>
#include <GLES2/gl2.h>
#include <sstream>
#include <android/log.h>
#include <mbgl/style/layers/custom_layer.hpp>

// DEBUGGING

const char* LOG_TAG = "Custom Layer Example";

const char* stringFromError(GLenum err) {
    switch (err) {
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";

        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";

        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";

        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";

        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";

#ifdef GL_TABLE_TOO_LARGE
        case GL_TABLE_TOO_LARGE:
        return "GL_TABLE_TOO_LARGE";
#endif

#ifdef GL_STACK_OVERFLOW
        case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW";
#endif

#ifdef GL_STACK_UNDERFLOW
        case GL_STACK_UNDERFLOW:
        return "GL_STACK_UNDERFLOW";
#endif

#ifdef GL_CONTEXT_LOST
        case GL_CONTEXT_LOST:
        return "GL_CONTEXT_LOST";
#endif

        default:
            return "GL_UNKNOWN";
    }
}

struct Error : std::runtime_error {
    using std::runtime_error::runtime_error;
};

void checkError(const char *cmd, const char *file, int line) {

    GLenum err = GL_NO_ERROR;
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::ostringstream message;
        message << cmd  << ": Error " << stringFromError(err);

        // Check for further errors
        while ((err = glGetError()) != GL_NO_ERROR) {
            message << ", " << stringFromError(err);
        }

        message << " at " << file << ":" << line;
        __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, message.str().c_str());
        throw Error(message.str());
    }
}

#ifndef NDEBUG
#define GL_CHECK_ERROR(cmd) ([&]() { struct __MBGL_C_E { ~__MBGL_C_E() noexcept(false) { checkError(#cmd, __FILE__, __LINE__); } } __MBGL_C_E; return cmd; }())
#else
#define GL_CHECK_ERROR(cmd) (cmd)
#endif

void checkLinkStatus(GLuint program) {
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        GLchar infoLog[maxLength];
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
        __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, &infoLog[0]);
        throw Error(infoLog);
    }

}

void checkCompileStatus(GLuint shader) {
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        GLchar errorLog[maxLength];
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
        __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, &errorLog[0]);
        throw Error(errorLog);
    }
}

// /DEBUGGING

static const GLchar * vertexShaderSource = "attribute vec2 a_pos; void main() { gl_Position = vec4(a_pos, 0, 1); }";
static const GLchar * fragmentShaderSource = "uniform highp vec4 fill_color; void main() { gl_FragColor = fill_color; }";

class ExampleCustomLayer: mbgl::style::CustomLayerHost {
public:
    ~ExampleCustomLayer() {
    }

    void initialize() {
        __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "Initialize");

        // Debug info
        int maxAttrib;
        GL_CHECK_ERROR(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttrib));
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Max vertex attributes: %i", maxAttrib);

        program = GL_CHECK_ERROR(glCreateProgram());
        vertexShader = GL_CHECK_ERROR(glCreateShader(GL_VERTEX_SHADER));
        fragmentShader = GL_CHECK_ERROR(glCreateShader(GL_FRAGMENT_SHADER));

        GL_CHECK_ERROR(glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr));
        GL_CHECK_ERROR(glCompileShader(vertexShader));
        checkCompileStatus(vertexShader);
        GL_CHECK_ERROR(glAttachShader(program, vertexShader));
        GL_CHECK_ERROR(glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr));
        GL_CHECK_ERROR(glCompileShader(fragmentShader));
        checkCompileStatus(fragmentShader);
        GL_CHECK_ERROR(glAttachShader(program, fragmentShader));
        GL_CHECK_ERROR(glLinkProgram(program));
        checkLinkStatus(program);

        a_pos = GL_CHECK_ERROR(glGetAttribLocation(program, "a_pos"));
        fill_color = GL_CHECK_ERROR(glGetUniformLocation(program, "fill_color"));

        GLfloat background[] = { -1, -1, 1, -1, -1, 1, 1, 1 };
        GL_CHECK_ERROR(glGenBuffers(1, &buffer));
        GL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, buffer));
        GL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), background, GL_STATIC_DRAW));
    }

    void render(const mbgl::style::CustomLayerRenderParameters&) {
        __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "Render");
        glUseProgram(program);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glEnableVertexAttribArray(a_pos);
        glVertexAttribPointer(a_pos, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);
        glUniform4fv(fill_color, 1, color);

        GL_CHECK_ERROR(glUseProgram(program));
        GL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, buffer));
        GL_CHECK_ERROR(glEnableVertexAttribArray(a_pos));
        GL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_FLOAT, GL_FALSE, 0, NULL));
        GL_CHECK_ERROR(glDisable(GL_STENCIL_TEST));
        GL_CHECK_ERROR(glDisable(GL_DEPTH_TEST));
        GL_CHECK_ERROR(glUniform4fv(fill_color, 1, color));
        GL_CHECK_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

    }

    void contextLost() {
        __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "ContextLost");
        program = 0;
    }

    void deinitialize() {
        __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "DeInitialize");
        if (program) {
            glDeleteBuffers(1, &buffer);
            glDetachShader(program, vertexShader);
            glDetachShader(program, fragmentShader);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(program);
        }
    }

    GLuint program = 0;
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    GLuint buffer = 0;
    GLuint a_pos = 0;
    GLuint fill_color = 0;

    static GLfloat color[];
};

GLfloat ExampleCustomLayer::color[] = { 0.0f, 1.0f, 0.0f, 1.0f };

jlong JNICALL nativeCreateContext(JNIEnv*, jobject) {
    __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "nativeCreateContext");
    auto exampleCustomLayer = std::make_unique<ExampleCustomLayer>();
    return reinterpret_cast<jlong>(exampleCustomLayer.release());
}

void JNICALL nativeSetColor(JNIEnv*, jobject, jfloat red, jfloat green, jfloat blue, jfloat alpha) {
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "nativeSetColor: %.2f, %.2f, %.2f, %.2f", red, green, blue, alpha);
    ExampleCustomLayer::color[0] = red;
    ExampleCustomLayer::color[1] = green;
    ExampleCustomLayer::color[2] = blue;
    ExampleCustomLayer::color[3] = alpha;
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "OnLoad");

    JNIEnv *env = nullptr;
    vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);

    jclass customLayerClass = env->FindClass("com/mapbox/mapboxsdk/testapp/model/customlayer/ExampleCustomLayer");

    JNINativeMethod methods[] = {
        {"createContext", "()J", reinterpret_cast<void *>(&nativeCreateContext)},
        {"setColor", "(FFFF)V", reinterpret_cast<void *>(&nativeSetColor)}
    };

    if (env->RegisterNatives(customLayerClass, methods, 2) < 0) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *, void *) {
}
