#include <mbgl/test/util.hpp>

#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;
using namespace mbgl::style;

static const GLchar* vertexShaderSource = R"MBGL_SHADER(
#ifdef GL_ES
precision mediump float;
#endif
attribute vec2 a_pos;
void main() {
    gl_Position = vec4(a_pos, 0, 1);
}
)MBGL_SHADER";

static const GLchar* fragmentShaderSource = R"MBGL_SHADER(
#ifdef GL_ES
precision mediump float;
#endif
void main() {
    gl_FragColor = vec4(0, 1, 0, 1);
}
)MBGL_SHADER";

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

TEST(GLContextMode, Shared) {
    util::RunLoop loop;

    DefaultFileSource fileSource(":memory:", "test/fixtures/api/assets");
    ThreadPool threadPool(4);
    float pixelRatio { 1 };

    HeadlessFrontend frontend { pixelRatio, fileSource, threadPool, {}, GLContextMode::Shared };

    Map map(frontend, MapObserver::nullObserver(), frontend.getSize(), pixelRatio, fileSource, threadPool, MapMode::Static);
    map.getStyle().loadJSON(util::read_file("test/fixtures/api/water.json"));
    map.setLatLngZoom({ 37.8, -122.5 }, 10);

    // Set transparent background layer.
    map.getStyle().getLayer("background")->as<BackgroundLayer>()->setBackgroundColor( { { 1.0f, 0.0f, 0.0f, 0.5f } } );

    {
        // Custom rendering outside of GL Native render loop.
        BackendScope scope { *frontend.getBackend() };
        frontend.getBackend()->bind();

        Shader paintShader(vertexShaderSource, fragmentShaderSource);
        Buffer triangleBuffer({ 0, 0.5, 0.5, -0.5, -0.5, -0.5 });
        MBGL_CHECK_ERROR(glUseProgram(paintShader.program));
        MBGL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, triangleBuffer.buffer));
        MBGL_CHECK_ERROR(glEnableVertexAttribArray(paintShader.a_pos));
        MBGL_CHECK_ERROR(glVertexAttribPointer(paintShader.a_pos, 2, GL_FLOAT, GL_FALSE, 0, nullptr));
        MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, 3));
    }

    test::checkImage("test/fixtures/shared_context", frontend.render(map), 0.5, 0.1);
}
