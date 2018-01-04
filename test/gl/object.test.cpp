#include <mbgl/test/util.hpp>

#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/context.hpp>

#include <memory>

using namespace mbgl;

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

TEST(GLObject, Value) {
    setFlag = false;

    auto object = std::make_unique<gl::State<MockGLObject>>();
    EXPECT_EQ(object->getCurrentValue(), false);
    EXPECT_TRUE(object->isDirty());
    EXPECT_FALSE(setFlag);

    *object = false;
    EXPECT_EQ(object->getCurrentValue(), false);
    EXPECT_FALSE(object->isDirty());
    EXPECT_TRUE(setFlag);

    setFlag = false;
    *object = true;
    EXPECT_EQ(object->getCurrentValue(), true);
    EXPECT_FALSE(object->isDirty());
    EXPECT_TRUE(setFlag);
}

TEST(GLObject, Store) {
    HeadlessBackend backend { { 256, 256 } };
    BackendScope scope { backend };

    gl::Context context;
    EXPECT_TRUE(context.empty());

    gl::UniqueTexture texture = context.createTexture();
    EXPECT_NE(texture.get(), 0u);
    texture.reset();
    EXPECT_FALSE(context.empty());
    context.performCleanup();
    EXPECT_FALSE(context.empty());
    context.reset();
    EXPECT_TRUE(context.empty());

    context.reset();
    EXPECT_TRUE(context.empty());
}
