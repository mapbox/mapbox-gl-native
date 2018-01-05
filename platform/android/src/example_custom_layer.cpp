#include <jni.h>
#include <GLES2/gl2.h>

#include <mbgl/util/logging.hpp>

#include <mbgl/style/layers/custom_layer.hpp>

static const GLchar * vertexShaderSource = "attribute vec2 a_pos; void main() { gl_Position = vec4(a_pos, 0, 1); }";
static const GLchar * fragmentShaderSource = "uniform vec4 fill_color; void main() { gl_FragColor = fill_color; }";

class ExampleCustomLayer {
public:
    ~ExampleCustomLayer() {
        mbgl::Log::Info(mbgl::Event::General, "~ExampleCustomLayer");
        if (program) {
            glDeleteBuffers(1, &buffer);
            glDetachShader(program, vertexShader);
            glDetachShader(program, fragmentShader);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(program);
        }
    }

    void initialize() {
        mbgl::Log::Info(mbgl::Event::General, "Initialize");
        program = glCreateProgram();
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        glAttachShader(program, vertexShader);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        a_pos = glGetAttribLocation(program, "a_pos");
        fill_color = glGetUniformLocation(program, "fill_color");

        GLfloat background[] = { -1,-1, 1,-1, -1,1, 1,1 };
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), background, GL_STATIC_DRAW);
    }

    void render() {
        mbgl::Log::Info(mbgl::Event::General, "Render");
        glUseProgram(program);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glEnableVertexAttribArray(a_pos);
        glVertexAttribPointer(a_pos, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);
        glUniform4fv(fill_color, 1, color);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
     mbgl::Log::Info(mbgl::Event::General, "nativeCreateContext");
     return reinterpret_cast<jlong>(new ExampleCustomLayer());
}

void JNICALL nativeSetColor(JNIEnv*, jobject, jfloat red, jfloat green, jfloat blue, jfloat alpha) {
    mbgl::Log::Info(mbgl::Event::General, "nativeSetColor: %.2f, %.2f, %.2f, %.2f", red, green, blue, alpha);
    ExampleCustomLayer::color[0] = red;
    ExampleCustomLayer::color[1] = green;
    ExampleCustomLayer::color[2] = blue;
    ExampleCustomLayer::color[3] = alpha;
}

void nativeInitialize(void *context) {
    mbgl::Log::Info(mbgl::Event::General, "nativeInitialize");
    reinterpret_cast<ExampleCustomLayer*>(context)->initialize();
}

void nativeRender(void *context, const mbgl::style::CustomLayerRenderParameters& /*parameters*/) {
    mbgl::Log::Info(mbgl::Event::General, "nativeRender");
    reinterpret_cast<ExampleCustomLayer*>(context)->render();
}

void nativeContextLost(void */*context*/) {
    mbgl::Log::Info(mbgl::Event::General, "nativeContextLost");
}

void nativeDeinitialize(void *context) {
    mbgl::Log::Info(mbgl::Event::General, "nativeDeinitialize");
    delete reinterpret_cast<ExampleCustomLayer*>(context);
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    mbgl::Log::Info(mbgl::Event::General, "OnLoad");
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

    env->SetStaticLongField(customLayerClass,
        env->GetStaticFieldID(customLayerClass, "InitializeFunction", "J"),
        reinterpret_cast<jlong>(nativeInitialize));

    env->SetStaticLongField(customLayerClass,
        env->GetStaticFieldID(customLayerClass, "RenderFunction", "J"),
        reinterpret_cast<jlong>(nativeRender));

    env->SetStaticLongField(customLayerClass,
        env->GetStaticFieldID(customLayerClass, "ContextLostFunction", "J"),
        reinterpret_cast<jlong>(nativeContextLost));

    env->SetStaticLongField(customLayerClass,
        env->GetStaticFieldID(customLayerClass, "DeinitializeFunction", "J"),
        reinterpret_cast<jlong>(nativeDeinitialize));

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *, void *) {
}
