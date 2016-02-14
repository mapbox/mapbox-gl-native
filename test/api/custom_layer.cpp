#include "../fixtures/util.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/layer/custom_layer.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/gl/gl_object_store.hpp>

using namespace mbgl;

static const GLchar * vertexShaderSource = "attribute vec2 a_pos; void main() { gl_Position = vec4(a_pos, 0, 1); }";
static const GLchar * fragmentShaderSource = "void main() { gl_FragColor = vec4(0, 1, 0, 1); }";

class TestLayer {
public:
    ~TestLayer() {
        if (program) {
            MBGL_CHECK_ERROR(glDetachShader(program.getID(), vertexShader.getID()));
            MBGL_CHECK_ERROR(glDetachShader(program.getID(), fragmentShader.getID()));
        }
    }

    void initialize() {
        program.create();
        vertexShader.create();
        fragmentShader.create();

        MBGL_CHECK_ERROR(glShaderSource(vertexShader.getID(), 1, &vertexShaderSource, nullptr));
        MBGL_CHECK_ERROR(glCompileShader(vertexShader.getID()));
        MBGL_CHECK_ERROR(glAttachShader(program.getID(), vertexShader.getID()));
        MBGL_CHECK_ERROR(glShaderSource(fragmentShader.getID(), 1, &fragmentShaderSource, nullptr));
        MBGL_CHECK_ERROR(glCompileShader(fragmentShader.getID()));
        MBGL_CHECK_ERROR(glAttachShader(program.getID(), fragmentShader.getID()));
        MBGL_CHECK_ERROR(glLinkProgram(program.getID()));
        a_pos = glGetAttribLocation(program.getID(), "a_pos");

        buffer.create();
        MBGL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, buffer.getID()));

        GLfloat background[] = { -1,-1, 1,-1,-1, 1, 1, 1 };
        MBGL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), background, GL_STATIC_DRAW));
    }

    void render() {
        MBGL_CHECK_ERROR(glUseProgram(program.getID()));
        MBGL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, buffer.getID()));
        MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_pos));
        MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_FLOAT, GL_FALSE, 0, NULL));
        MBGL_CHECK_ERROR(glDisable(GL_STENCIL_TEST));
        MBGL_CHECK_ERROR(glDisable(GL_DEPTH_TEST));
        MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
    }

    gl::ProgramHolder program;
    gl::ShaderHolder vertexShader = { GL_VERTEX_SHADER };
    gl::ShaderHolder fragmentShader = { GL_FRAGMENT_SHADER };
    gl::BufferHolder buffer;
    GLuint a_pos = 0;
};

TEST(CustomLayer, Basic) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    OnlineFileSource fileSource;

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addCustomLayer(
        "custom",
        [] (void* context) {
            reinterpret_cast<TestLayer*>(context)->initialize();
        },
        [] (void* context, const CustomLayerRenderParameters&) {
            reinterpret_cast<TestLayer*>(context)->render();
        },
        [] (void* context) {
            delete reinterpret_cast<TestLayer*>(context);
        }, new TestLayer());

    test::checkImage("test/fixtures/custom_layer/basic", test::render(map));
}
