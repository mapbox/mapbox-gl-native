#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/sprite/sprite_image_collection.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/string.hpp>

#include <utility>

using namespace mbgl;

TEST(SpriteImageCollection, OtherPixelRatio) {
    FixtureLog log;
    Images images;

    // Adding mismatched sprite image
    addSpriteImage(images, "one", std::make_unique<style::Image>(PremultipliedImage({ 8, 8 }), 2));
}

TEST(SpriteImageCollection, Replace) {
    FixtureLog log;
    Images images;

    addSpriteImage(images, "sprite", std::make_unique<style::Image>(PremultipliedImage({ 16, 16 }), 2));
    auto image = images.find("sprite")->second.get();
    addSpriteImage(images, "sprite", std::make_unique<style::Image>(PremultipliedImage({ 16, 16 }), 2));
    EXPECT_NE(image, images.find("sprite")->second.get());
}

TEST(SpriteImageCollection, ReplaceWithDifferentDimensions) {
    FixtureLog log;
    Images images;

    addSpriteImage(images, "sprite", std::make_unique<style::Image>(PremultipliedImage({ 16, 16 }), 2));
    addSpriteImage(images, "sprite", std::make_unique<style::Image>(PremultipliedImage({ 18, 18 }), 2));

    EXPECT_EQ(1u, log.count({
                                    EventSeverity::Warning,
                                    Event::Sprite,
                                    int64_t(-1),
                                    "Can't change sprite dimensions for 'sprite'",
                            }));
}


