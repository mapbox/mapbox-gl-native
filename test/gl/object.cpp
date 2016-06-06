#include <mbgl/test/util.hpp>

#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>

#include <mbgl/gl/gl_helper.hpp>
#include <mbgl/gl/gl_config.hpp>
#include <mbgl/gl/object_store.hpp>
#include <mbgl/gl/texture_pool.hpp>

#include <memory>

namespace {

static bool getFlag = false;
static bool setFlag = false;

}; // namespace

struct MockGLObject {
    using Type = bool;
    static const Type Default = false;
    static Type Get() { getFlag = true; return true; }
    static void Set(const Type&) { setFlag = true; }
};

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
    EXPECT_TRUE(object->getDirty());
    EXPECT_TRUE(setFlag);
}

TEST(GLObject, Store) {
    mbgl::HeadlessView view(std::make_shared<mbgl::HeadlessDisplay>(), 1);
    view.activate();

    mbgl::gl::ObjectStore store;
    EXPECT_TRUE(store.empty());

    mbgl::gl::UniqueProgram program = store.createProgram();
    EXPECT_NE(program.get(), 0);
    program.reset();
    EXPECT_FALSE(store.empty());
    store.performCleanup();
    EXPECT_TRUE(store.empty());

    mbgl::gl::UniqueShader shader = store.createShader(GL_VERTEX_SHADER);
    EXPECT_NE(shader.get(), 0);
    shader.reset();
    EXPECT_FALSE(store.empty());
    store.performCleanup();
    EXPECT_TRUE(store.empty());

    mbgl::gl::UniqueBuffer buffer = store.createBuffer();
    EXPECT_NE(buffer.get(), 0);
    buffer.reset();
    EXPECT_FALSE(store.empty());
    store.performCleanup();
    EXPECT_TRUE(store.empty());

    mbgl::gl::UniqueTexture texture = store.createTexture();
    EXPECT_NE(texture.get(), 0);
    texture.reset();
    EXPECT_FALSE(store.empty());
    store.performCleanup();
    EXPECT_TRUE(store.empty());

    mbgl::gl::UniqueVAO vao = store.createVAO();
    EXPECT_NE(vao.get(), 0);
    vao.reset();
    EXPECT_FALSE(store.empty());
    store.performCleanup();
    EXPECT_TRUE(store.empty());

    mbgl::gl::UniqueTexturePool texturePool = store.createTexturePool();
    for (auto& id : texturePool.get()) {
        EXPECT_NE(id, 0);
    }
    EXPECT_TRUE(texturePool.get().size() == size_t(mbgl::gl::TextureMax));
    texturePool.reset();
    EXPECT_FALSE(store.empty());
    store.performCleanup();
    EXPECT_TRUE(store.empty());

    view.deactivate();
}

TEST(GLObject, TexturePool) {
    mbgl::HeadlessView view(std::make_shared<mbgl::HeadlessDisplay>(), 1);
    view.activate();

    mbgl::gl::ObjectStore store;
    EXPECT_TRUE(store.empty());

    mbgl::gl::TexturePool pool;

    std::vector<mbgl::gl::SharedTexture> ids;

    // Fill an entire texture pool.
    for (auto i = 0; i != mbgl::gl::TextureMax; ++i) {
        ids.push_back(pool.acquireTexture(store));
        EXPECT_EQ(ids.back().get(), GLuint(i + 1));
        EXPECT_TRUE(store.empty());
    }

    // Reuse texture ids from the same pool.
    for (auto i = 0; i != mbgl::gl::TextureMax; ++i) {
        ids[i].reset();
        ids.push_back(pool.acquireTexture(store));
        EXPECT_EQ(ids.back().get(), GLuint(i + 1));
        EXPECT_TRUE(store.empty());
    }

    // Trigger a new texture pool creation.
    {
        mbgl::gl::SharedTexture id = pool.acquireTexture(store);
        EXPECT_EQ(id, mbgl::gl::TextureMax + 1);
        EXPECT_TRUE(store.empty());

        id.reset();

        // Last used texture from pool triggers pool recycling.
        EXPECT_FALSE(store.empty());

        store.performCleanup();
        EXPECT_TRUE(store.empty());
    }

    // First pool is still full, thus creating a new pool.
    mbgl::gl::SharedTexture id1 = pool.acquireTexture(store);
    EXPECT_GT(id1.get(), mbgl::gl::TextureMax);
    EXPECT_TRUE(store.empty());

    // Release all textures from the first pool.
    ids.clear();
    EXPECT_FALSE(store.empty());

    store.performCleanup();
    EXPECT_TRUE(store.empty());

    // The first pool is now gone, the next pool is now in use.
    mbgl::gl::SharedTexture id2 = pool.acquireTexture(store);
    EXPECT_GT(id2.get(), id1.get());

    id2.reset();
    EXPECT_TRUE(store.empty());

    // Last used texture from the pool triggers pool recycling.
    id1.reset();
    EXPECT_FALSE(store.empty());

    store.performCleanup();
    EXPECT_TRUE(store.empty());

    view.deactivate();
}
