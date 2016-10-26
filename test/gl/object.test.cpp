#include <mbgl/test/util.hpp>

#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>

#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/context.hpp>

#include <memory>

namespace {

static bool getFlag = false;
static bool setFlag = false;

} // namespace

struct MockGLObject {
    using Type = bool;
    static const Type Default;
    static Type Get() { getFlag = true; return true; }
    static void Set(const Type&) { setFlag = true; }
};

const bool MockGLObject::Default = false;

TEST(GLObject, PreserveState) {
    getFlag = false;
    setFlag = false;

    auto object = std::make_unique<mbgl::gl::PreserveState<MockGLObject>>();
    EXPECT_TRUE(getFlag);
    EXPECT_FALSE(setFlag);

    getFlag = false;
    object.reset();
    EXPECT_FALSE(getFlag);
    EXPECT_TRUE(setFlag);
}

TEST(GLObject, Value) {
    setFlag = false;

    auto object = std::make_unique<mbgl::gl::State<MockGLObject>>();
    EXPECT_EQ(object->getCurrentValue(), false);
    EXPECT_FALSE(object->isDirty());
    EXPECT_FALSE(setFlag);

    object->setDirty();
    EXPECT_TRUE(object->isDirty());

    *object = false;
    EXPECT_EQ(object->getCurrentValue(), false);
    EXPECT_FALSE(object->isDirty());
    EXPECT_TRUE(setFlag);

    setFlag = false;
    *object = true;
    EXPECT_EQ(object->getCurrentValue(), true);
    EXPECT_FALSE(object->isDirty());
    EXPECT_TRUE(setFlag);

    object->reset();
    EXPECT_EQ(object->getCurrentValue(), false);
    EXPECT_FALSE(object->isDirty());
    EXPECT_TRUE(setFlag);
}

TEST(GLObject, Store) {
    mbgl::HeadlessView view(std::make_shared<mbgl::HeadlessDisplay>(), 1);
    view.activate();

    mbgl::gl::Context context;
    EXPECT_TRUE(context.empty());

    mbgl::gl::UniqueProgram program = context.createProgram();
    EXPECT_NE(program.get(), 0u);
    program.reset();
    EXPECT_FALSE(context.empty());
    context.performCleanup();
    EXPECT_TRUE(context.empty());

    mbgl::gl::UniqueShader shader = context.createVertexShader();
    EXPECT_NE(shader.get(), 0u);
    shader.reset();
    EXPECT_FALSE(context.empty());
    context.performCleanup();
    EXPECT_TRUE(context.empty());

    mbgl::gl::UniqueTexture texture = context.createTexture();
    EXPECT_NE(texture.get(), 0u);
    texture.reset();
    EXPECT_FALSE(context.empty());
    context.performCleanup();
    EXPECT_FALSE(context.empty());
    context.reset();
    EXPECT_TRUE(context.empty());

    mbgl::gl::UniqueVertexArray vao = context.createVertexArray();
    EXPECT_NE(vao.get(), 0u);
    vao.reset();
    EXPECT_FALSE(context.empty());
    context.performCleanup();
    EXPECT_TRUE(context.empty());

    context.reset();
    EXPECT_TRUE(context.empty());

    view.deactivate();
}
