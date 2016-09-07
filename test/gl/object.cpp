#include <mbgl/test/util.hpp>

#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>

#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/gl_helper.hpp>
#include <mbgl/gl/gl_config.hpp>
#include <mbgl/gl/object_store.hpp>

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

TEST(GLObject, Preserve) {
    getFlag = false;
    setFlag = false;

    auto object = std::make_unique<mbgl::gl::Preserve<MockGLObject>>();
    EXPECT_TRUE(getFlag);
    EXPECT_FALSE(setFlag);

    getFlag = false;
    object.reset();
    EXPECT_FALSE(getFlag);
    EXPECT_TRUE(setFlag);
}

TEST(GLObject, Value) {
    setFlag = false;

    auto object = std::make_unique<mbgl::gl::Value<MockGLObject>>();
    EXPECT_EQ(object->getCurrent(), false);
    EXPECT_FALSE(object->getDirty());
    EXPECT_FALSE(setFlag);

    object->setDirty();
    EXPECT_TRUE(object->getDirty());

    *object = false;
    EXPECT_EQ(object->getCurrent(), false);
    EXPECT_FALSE(object->getDirty());
    EXPECT_TRUE(setFlag);

    setFlag = false;
    *object = true;
    EXPECT_EQ(object->getCurrent(), true);
    EXPECT_FALSE(object->getDirty());
    EXPECT_TRUE(setFlag);

    object->reset();
    EXPECT_EQ(object->getCurrent(), false);
    EXPECT_FALSE(object->getDirty());
    EXPECT_TRUE(setFlag);
}

TEST(GLObject, Store) {
    mbgl::HeadlessView view(std::make_shared<mbgl::HeadlessDisplay>(), 1);
    view.activate();

    mbgl::gl::ObjectStore store;
    EXPECT_TRUE(store.empty());

    mbgl::gl::UniqueProgram program = store.createProgram();
    EXPECT_NE(program.get(), 0u);
    program.reset();
    EXPECT_FALSE(store.empty());
    store.performCleanup();
    EXPECT_TRUE(store.empty());

    mbgl::gl::UniqueShader shader = store.createShader(GL_VERTEX_SHADER);
    EXPECT_NE(shader.get(), 0u);
    shader.reset();
    EXPECT_FALSE(store.empty());
    store.performCleanup();
    EXPECT_TRUE(store.empty());

    mbgl::gl::UniqueBuffer buffer = store.createBuffer();
    EXPECT_NE(buffer.get(), 0u);
    buffer.reset();
    EXPECT_FALSE(store.empty());
    store.performCleanup();
    EXPECT_TRUE(store.empty());

    mbgl::gl::UniqueTexture texture = store.createTexture();
    EXPECT_NE(texture.get(), 0u);
    texture.reset();
    EXPECT_FALSE(store.empty());
    store.performCleanup();
    EXPECT_FALSE(store.empty());
    store.reset();
    EXPECT_TRUE(store.empty());

    mbgl::gl::UniqueVAO vao = store.createVAO();
    EXPECT_NE(vao.get(), 0u);
    vao.reset();
    EXPECT_FALSE(store.empty());
    store.performCleanup();
    EXPECT_TRUE(store.empty());

    store.reset();
    EXPECT_TRUE(store.empty());

    view.deactivate();
}
