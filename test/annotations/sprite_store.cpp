#include "../fixtures/util.hpp"

#include <mbgl/annotation/sprite_store.hpp>

using namespace mbgl;

TEST(Annotations, SpriteStore) {
    const auto sprite1 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));
    const auto sprite2 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));
    const auto sprite3 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));

    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;
    SpriteStore store(2);

    // Adding single
    store.setSprite("one", sprite1);
    EXPECT_EQ(Sprites({
        { "one", sprite1 },
    }), store.getDirty());
    EXPECT_EQ(Sprites(), store.getDirty());

    // Adding multiple
    store.setSprite("two", sprite2);
    store.setSprite("three", sprite3);
    EXPECT_EQ(Sprites({
        { "two", sprite2 },
        { "three", sprite3 },
    }), store.getDirty());
    EXPECT_EQ(Sprites(), store.getDirty());

    // Removing
    store.removeSprite("one");
    store.removeSprite("two");
    EXPECT_EQ(Sprites({
        { "one", nullptr },
        { "two", nullptr },
    }), store.getDirty());
    EXPECT_EQ(Sprites(), store.getDirty());

    // Accessing
    EXPECT_EQ(sprite3, store.getSprite("three"));
    EXPECT_EQ(nullptr, store.getSprite("two"));
    EXPECT_EQ(nullptr, store.getSprite("four"));

    // Overwriting
    store.setSprite("three", sprite1);
    EXPECT_EQ(Sprites({
        { "three", sprite1 },
    }), store.getDirty());
    EXPECT_EQ(Sprites(), store.getDirty());
}
