#include <jni.h>
#include <GLES2/gl2.h>

#include <mbgl/style/layers/custom_layer.hpp>

static const GLchar * vertexShaderSource = "attribute vec2 a_pos; void main() { gl_Position = vec4(a_pos, 0, 1); }";
static const GLchar * fragmentShaderSource = "void main() { gl_FragColor = vec4(0, 1, 0, 1); }";

class ExampleCustomLayer {
public:
    ~ExampleCustomLayer() {
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

        GLfloat background[] = { -1,-1, 1,-1, -1,1, 1,1 };
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), background, GL_STATIC_DRAW);
    }

    void render() {
        glUseProgram(program);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glEnableVertexAttribArray(a_pos);
        glVertexAttribPointer(a_pos, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    GLuint program = 0;
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    GLuint buffer = 0;
    GLuint a_pos = 0;
};

jlong JNICALL nativeCreateContext(JNIEnv*, jobject) {
    return reinterpret_cast<jlong>(new ExampleCustomLayer());
}

void nativeInitialize(void *context) {
    reinterpret_cast<ExampleCustomLayer*>(context)->initialize();
}

void nativeRender(void *context, const mbgl::style::CustomLayerRenderParameters& /*parameters*/) {
    reinterpret_cast<ExampleCustomLayer*>(context)->render();
}

void nativeDenitialize(void *context) {
    delete reinterpret_cast<ExampleCustomLayer*>(context);
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env = nullptr;
    vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);

    jclass customLayerClass = env->FindClass("com/mapbox/mapboxsdk/testapp/model/customlayer/ExampleCustomLayer");

    JNINativeMethod methods[] = {
        {"createContext", "()J", reinterpret_cast<void *>(&nativeCreateContext)}
    };

    if (env->RegisterNatives(customLayerClass, methods, 1) < 0) {
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
        env->GetStaticFieldID(customLayerClass, "DeinitializeFunction", "J"),
        reinterpret_cast<jlong>(nativeDenitialize));

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *, void *) {
}
