#include <mbgl/test/util.hpp>

#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/gl/custom_layer.hpp>
#include <mbgl/gl/defines.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/platform/gl_functions.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::platform;

// Note that custom layers need to draw geometry with a z value of 1 to take advantage of
// depth-based fragment culling.
static const GLchar* vertexShaderSource = R"MBGL_SHADER(
attribute vec2 a_pos;
void main() {
    gl_Position = vec4(a_pos, 1, 1);
}
)MBGL_SHADER";

static const GLchar* fragmentShaderSource = R"MBGL_SHADER(
void main() {
    gl_FragColor = vec4(0, 0.5, 0, 0.5);
}
)MBGL_SHADER";

// Not using any mbgl-specific stuff (other than a basic error-checking macro) in the
// layer implementation because it is intended to reflect how someone using custom layers
// might actually write their own implementation.

class TestLayer : public mbgl::style::CustomLayerHost {
public:
    void initialize() override {
        program = MBGL_CHECK_ERROR(glCreateProgram());
        vertexShader = MBGL_CHECK_ERROR(glCreateShader(GL_VERTEX_SHADER));
        fragmentShader = MBGL_CHECK_ERROR(glCreateShader(GL_FRAGMENT_SHADER));

        MBGL_CHECK_ERROR(glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr));
        MBGL_CHECK_ERROR(glCompileShader(vertexShader));
        MBGL_CHECK_ERROR(glAttachShader(program, vertexShader));
        MBGL_CHECK_ERROR(glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr));
        MBGL_CHECK_ERROR(glCompileShader(fragmentShader));
        MBGL_CHECK_ERROR(glAttachShader(program, fragmentShader));
        MBGL_CHECK_ERROR(glLinkProgram(program));
        a_pos = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_pos"));

        GLfloat triangle[] = { 0, 0.5, 0.5, -0.5, -0.5, -0.5 };
        MBGL_CHECK_ERROR(glGenBuffers(1, &buffer));
        MBGL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, buffer));
        MBGL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), triangle, GL_STATIC_DRAW));
    }

    void render(const mbgl::style::CustomLayerRenderParameters&) override {
        MBGL_CHECK_ERROR(glUseProgram(program));
        MBGL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, buffer));
        MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_pos));
        MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_FLOAT, GL_FALSE, 0, nullptr));
        MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, 3));
    }

    void contextLost() override {}

    void deinitialize() override {
         if (program) {
                MBGL_CHECK_ERROR(glDeleteBuffers(1, &buffer));
                MBGL_CHECK_ERROR(glDetachShader(program, vertexShader));
                MBGL_CHECK_ERROR(glDetachShader(program, fragmentShader));
                MBGL_CHECK_ERROR(glDeleteShader(vertexShader));
                MBGL_CHECK_ERROR(glDeleteShader(fragmentShader));
                MBGL_CHECK_ERROR(glDeleteProgram(program));
            }
    }

    GLuint program = 0;
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    GLuint buffer = 0;
    GLuint a_pos = 0;
};

TEST(CustomLayer, Basic) {
    if (gfx::Backend::GetType() != gfx::Backend::Type::OpenGL) {
        return;
    }

    util::RunLoop loop;

    HeadlessFrontend frontend { 1 };
    Map map(frontend, MapObserver::nullObserver(),
            MapOptions().withMapMode(MapMode::Static).withSize(frontend.getSize()),
            ResourceOptions().withCachePath(":memory:").withAssetPath("test/fixtures/api/assets"));
    map.getStyle().loadJSON(util::read_file("test/fixtures/api/water.json"));
    map.jumpTo(CameraOptions().withCenter(LatLng { 37.8, -122.5 }).withZoom(10.0));
    map.getStyle().addLayer(std::make_unique<CustomLayer>(
        "custom",
        std::make_unique<TestLayer>()));

    auto layer = std::make_unique<FillLayer>("landcover", "mapbox");
    layer->setSourceLayer("landcover");
    layer->setFillColor(Color{ 1.0, 1.0, 0.0, 1.0 });
    map.getStyle().addLayer(std::move(layer));

    test::checkImage("test/fixtures/custom_layer/basic", frontend.render(map).image, 0.0006, 0.1);
}
