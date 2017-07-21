#include <mbgl/test/util.hpp>

#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/renderer/backend_scope.hpp>

#include <mbgl/util/offscreen_texture.hpp>

using namespace mbgl;

TEST(OffscreenTexture, EmptyRed) {
    HeadlessBackend backend({ 512, 256 });
    BackendScope scope { backend };

    // Scissor test shouldn't leak after HeadlessBackend::bind().
    MBGL_CHECK_ERROR(glScissor(64, 64, 128, 128));
    backend.getContext().scissorTest.setCurrentValue(true);

    backend.bind();

    MBGL_CHECK_ERROR(glClearColor(1.0f, 0.0f, 0.0f, 1.0f));
    MBGL_CHECK_ERROR(glClear(GL_COLOR_BUFFER_BIT));

    auto image = backend.readStillImage();
    test::checkImage("test/fixtures/offscreen_texture/empty-red", image, 0, 0);
}

struct Shader {
    Shader(const GLchar* vertex, const GLchar* fragment) {
        program = MBGL_CHECK_ERROR(glCreateProgram());
        vertexShader = MBGL_CHECK_ERROR(glCreateShader(GL_VERTEX_SHADER));
        fragmentShader = MBGL_CHECK_ERROR(glCreateShader(GL_FRAGMENT_SHADER));
        MBGL_CHECK_ERROR(glShaderSource(vertexShader, 1, &vertex, nullptr));
        MBGL_CHECK_ERROR(glCompileShader(vertexShader));
        MBGL_CHECK_ERROR(glAttachShader(program, vertexShader));
        MBGL_CHECK_ERROR(glShaderSource(fragmentShader, 1, &fragment, nullptr));
        MBGL_CHECK_ERROR(glCompileShader(fragmentShader));
        MBGL_CHECK_ERROR(glAttachShader(program, fragmentShader));
        MBGL_CHECK_ERROR(glLinkProgram(program));
        a_pos = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_pos"));
    }

    ~Shader() {
        MBGL_CHECK_ERROR(glDetachShader(program, vertexShader));
        MBGL_CHECK_ERROR(glDetachShader(program, fragmentShader));
        MBGL_CHECK_ERROR(glDeleteShader(vertexShader));
        MBGL_CHECK_ERROR(glDeleteShader(fragmentShader));
        MBGL_CHECK_ERROR(glDeleteProgram(program));
    }

    GLuint program = 0;
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    GLuint a_pos = 0;
};

struct Buffer {
    Buffer(std::vector<GLfloat> data) {
        MBGL_CHECK_ERROR(glGenBuffers(1, &buffer));
        MBGL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, buffer));
        MBGL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(),
                                      GL_STATIC_DRAW));
    }

    ~Buffer() {
        MBGL_CHECK_ERROR(glDeleteBuffers(1, &buffer));
    }

    GLuint buffer = 0;
};


TEST(OffscreenTexture, RenderToTexture) {
    HeadlessBackend backend({ 512, 256 });
    BackendScope scope { backend };
    auto& context = backend.getContext();

    MBGL_CHECK_ERROR(glEnable(GL_BLEND));
    MBGL_CHECK_ERROR(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    Shader paintShader(R"MBGL_SHADER(
#ifdef GL_ES
precision mediump float;
#endif
attribute vec2 a_pos;
void main() {
    gl_Position = vec4(a_pos, 0, 1);
}
)MBGL_SHADER", R"MBGL_SHADER(
#ifdef GL_ES
precision mediump float;
#endif
void main() {
    gl_FragColor = vec4(0, 0.8, 0, 0.8);
}
)MBGL_SHADER");

        Shader compositeShader(R"MBGL_SHADER(
#ifdef GL_ES
precision mediump float;
#endif
attribute vec2 a_pos;
varying vec2 v_texcoord;
void main() {
    gl_Position = vec4(a_pos, 0, 1);
    v_texcoord = (a_pos + 1.0) / 2.0;
}
)MBGL_SHADER", R"MBGL_SHADER(
#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D u_texture;
varying vec2 v_texcoord;
void main() {
    gl_FragColor = texture2D(u_texture, v_texcoord);
}
)MBGL_SHADER");

    GLuint u_texture = MBGL_CHECK_ERROR(glGetUniformLocation(compositeShader.program, "u_texture"));

    Buffer triangleBuffer({ 0, 0.5, 0.5, -0.5, -0.5, -0.5 });
    Buffer viewportBuffer({ -1, -1, 1, -1, -1, 1, 1, 1 });

    backend.bind();

    // First, draw red to the bound FBO.
    context.clear(Color::red(), {}, {});

    // Then, create a texture, bind it, and render yellow to that texture. This should not
    // affect the originally bound FBO.
    OffscreenTexture texture(context, { 128, 128 });

    // Scissor test shouldn't leak after OffscreenTexture::bind().
    MBGL_CHECK_ERROR(glScissor(32, 32, 64, 64));
    context.scissorTest.setCurrentValue(true);

    texture.bind();

    context.clear(Color(), {}, {});

    MBGL_CHECK_ERROR(glUseProgram(paintShader.program));
    MBGL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, triangleBuffer.buffer));
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(paintShader.a_pos));
    MBGL_CHECK_ERROR(
        glVertexAttribPointer(paintShader.a_pos, 2, GL_FLOAT, GL_FALSE, 0, nullptr));
    MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, 3));

    auto image = texture.readStillImage();
    test::checkImage("test/fixtures/offscreen_texture/render-to-texture", image, 0, 0);

    // Now reset the FBO back to normal and retrieve the original (restored) framebuffer.
    backend.bind();

    image = backend.readStillImage();
    test::checkImage("test/fixtures/offscreen_texture/render-to-fbo", image, 0, 0);

    // Now, composite the Framebuffer texture we've rendered to onto the main FBO.
    context.bindTexture(texture.getTexture(), 0, gl::TextureFilter::Linear);
    MBGL_CHECK_ERROR(glUseProgram(compositeShader.program));
    MBGL_CHECK_ERROR(glUniform1i(u_texture, 0));
    MBGL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, viewportBuffer.buffer));
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(compositeShader.a_pos));
    MBGL_CHECK_ERROR(
        glVertexAttribPointer(compositeShader.a_pos, 2, GL_FLOAT, GL_FALSE, 0, nullptr));
    MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

    image = backend.readStillImage();
    test::checkImage("test/fixtures/offscreen_texture/render-to-fbo-composited", image, 0, 0.1);
}
