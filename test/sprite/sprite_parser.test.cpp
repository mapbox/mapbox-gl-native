#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/string.hpp>

#include <algorithm>

using namespace mbgl;

namespace {

auto readImage(const std::string& name) {
    return decodeImage(util::read_file(name));
}

} // namespace

TEST(Sprite, SpriteImageCreationInvalid) {
    FixtureLog log;

    const PremultipliedImage image_1x = decodeImage(util::read_file("test/fixtures/annotations/emerald.png"));

    ASSERT_EQ(200u, image_1x.size.width);
    ASSERT_EQ(299u, image_1x.size.height);

    ASSERT_EQ(nullptr, createStyleImage("test", image_1x, 0, 0, 0, 16, 1, false));    // width == 0
    ASSERT_EQ(nullptr, createStyleImage("test", image_1x, 0, 0, 16, 0, 1, false));    // height == 0
    ASSERT_EQ(nullptr, createStyleImage("test", image_1x, 0, 0, -1, 16, 1, false));   // width < 0
    ASSERT_EQ(nullptr, createStyleImage("test", image_1x, 0, 0, 16, -1, 1, false));   // height < 0
    ASSERT_EQ(nullptr, createStyleImage("test", image_1x, 0, 0, 1, 1, 0, false));     // ratio == 0
    ASSERT_EQ(nullptr, createStyleImage("test", image_1x, 0, 0, 1, 1, -1, false));    // ratio < 0
    ASSERT_EQ(nullptr, createStyleImage("test", image_1x, 0, 0, 1, 1, 23, false));    // ratio too large
    ASSERT_EQ(nullptr, createStyleImage("test", image_1x, 0, 0, 2048, 16, 1, false)); // too wide
    ASSERT_EQ(nullptr, createStyleImage("test", image_1x, 0, 0, 16, 1025, 1, false)); // too tall
    ASSERT_EQ(nullptr, createStyleImage("test", image_1x, -1, 0, 16, 16, 1, false));  // srcX < 0
    ASSERT_EQ(nullptr, createStyleImage("test", image_1x, 0, -1, 16, 16, 1, false));  // srcY < 0
    ASSERT_EQ(nullptr, createStyleImage("test", image_1x, 0, 0, image_1x.size.width + 1, 16, 1, false));   // right edge out of bounds
    ASSERT_EQ(nullptr, createStyleImage("test", image_1x, 0, 0, 16, image_1x.size.height + 1, 1, false));  // bottom edge out of bounds

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 0x16@0,0 in 200x299@1x sprite",
                  }));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 16x0@0,0 in 200x299@1x sprite",
                  }));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 4294967295x16@0,0 in 200x299@1x sprite",
                  }));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 16x4294967295@0,0 in 200x299@1x sprite",
                  }));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 1x1@0,0 in 200x299@0x sprite",
                  }));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 1x1@0,0 in 200x299@-1x sprite",
                  }));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 1x1@0,0 in 200x299@23x sprite",
                  }));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 2048x16@0,0 in 200x299@1x sprite",
                  }));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 16x1025@0,0 in 200x299@1x sprite",
                  }));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 16x16@4294967295,0 in 200x299@1x sprite",
                  }));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 16x16@0,4294967295 in 200x299@1x sprite",
                  }));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 201x16@0,0 in 200x299@1x sprite",
                  }));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 16x300@0,0 in 200x299@1x sprite",
                  }));

}

TEST(Sprite, SpriteImageCreation1x) {
    const PremultipliedImage image_1x = decodeImage(util::read_file("test/fixtures/annotations/emerald.png"));

    ASSERT_EQ(200u, image_1x.size.width);
    ASSERT_EQ(299u, image_1x.size.height);

    { // "museum_icon":{"x":177,"y":187,"width":18,"height":18,"pixelRatio":1,"sdf":false}
        const auto sprite = createStyleImage("test", image_1x, 177, 187, 18, 18, 1, false);
        ASSERT_TRUE(sprite.get());
        EXPECT_EQ(18u, sprite->getImage().size.width);
        EXPECT_EQ(18u, sprite->getImage().size.height);
        EXPECT_EQ(1, sprite->getPixelRatio());
        EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteimagecreation1x-museum.png"),
                  sprite->getImage());
    }
}

TEST(Sprite, SpriteImageCreation2x) {
    const PremultipliedImage image_2x = decodeImage(util::read_file("test/fixtures/annotations/emerald@2x.png"));

    // "museum_icon":{"x":354,"y":374,"width":36,"height":36,"pixelRatio":2,"sdf":false}
    const auto sprite = createStyleImage("test", image_2x, 354, 374, 36, 36, 2, false);
    ASSERT_TRUE(sprite.get());
    EXPECT_EQ(36u, sprite->getImage().size.width);
    EXPECT_EQ(36u, sprite->getImage().size.height);
    EXPECT_EQ(2, sprite->getPixelRatio());
    EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteimagecreation2x.png"),
              sprite->getImage());
}

TEST(Sprite, SpriteImageCreation1_5x) {
    const PremultipliedImage image_2x = decodeImage(util::read_file("test/fixtures/annotations/emerald@2x.png"));

    // "museum_icon":{"x":354,"y":374,"width":36,"height":36,"pixelRatio":2,"sdf":false}
    const auto sprite = createStyleImage("test", image_2x, 354, 374, 36, 36, 1.5, false);
    ASSERT_TRUE(sprite.get());
    EXPECT_EQ(36u, sprite->getImage().size.width);
    EXPECT_EQ(36u, sprite->getImage().size.height);
    EXPECT_EQ(1.5, sprite->getPixelRatio());
    EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteimagecreation1_5x-museum.png"),
              sprite->getImage());

    // "hospital_icon":{"x":314,"y":518,"width":36,"height":36,"pixelRatio":2,"sdf":false}
    const auto sprite2 = createStyleImage("test", image_2x, 314, 518, 35, 35, 1.5, false);
    ASSERT_TRUE(sprite2.get());
    EXPECT_EQ(35u, sprite2->getImage().size.width);
    EXPECT_EQ(35u, sprite2->getImage().size.height);
    EXPECT_EQ(1.5, sprite2->getPixelRatio());
    EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteimagecreation1_5x-hospital.png"),
              sprite2->getImage());
}

TEST(Sprite, SpriteParsing) {
    const auto image_1x = util::read_file("test/fixtures/annotations/emerald.png");
    const auto json_1x = util::read_file("test/fixtures/annotations/emerald.json");

    const auto images = parseSprite(image_1x, json_1x);

    std::set<std::string> names;
    std::transform(images.begin(), images.end(), std::inserter(names, names.begin()),
                   [](const auto& image) { return image->getID(); });

    EXPECT_EQ(std::set<std::string>({ "airfield_icon",
                                      "airport_icon",
                                      "background",
                                      "cemetery_icon",
                                      "college_icon",
                                      "default_1",
                                      "default_2",
                                      "default_3",
                                      "default_4",
                                      "default_5",
                                      "default_6",
                                      "default_marker",
                                      "dlr",
                                      "dlr.london-overground.london-underground.national-rail",
                                      "dlr.london-underground",
                                      "dlr.london-underground.national-rail",
                                      "dlr.national-rail",
                                      "dot",
                                      "embassy_icon",
                                      "fire-station_icon",
                                      "generic-metro",
                                      "generic-rail",
                                      "generic_icon",
                                      "golf_icon",
                                      "government_icon",
                                      "grass_pattern",
                                      "harbor_icon",
                                      "hospital_icon",
                                      "hospital_striped",
                                      "interstate_1",
                                      "interstate_2",
                                      "interstate_3",
                                      "library_icon",
                                      "london-overground",
                                      "london-overground.london-underground",
                                      "london-overground.london-underground.national-rail",
                                      "london-overground.national-rail",
                                      "london-underground",
                                      "london-underground.national-rail",
                                      "marker_icon",
                                      "metro",
                                      "metro.rer",
                                      "monument_icon",
                                      "moscow-metro",
                                      "museum_icon",
                                      "national-rail",
                                      "oneway_motorway",
                                      "oneway_road",
                                      "park_icon",
                                      "police_icon",
                                      "post_icon",
                                      "prison_icon",
                                      "religious-christian_icon",
                                      "religious-jewish_icon",
                                      "religious-muslim_icon",
                                      "rer",
                                      "rer.transilien",
                                      "s-bahn",
                                      "s-bahn.u-bahn",
                                      "sand_noise",
                                      "school_icon",
                                      "school_striped",
                                      "secondary_marker",
                                      "u-bahn",
                                      "us_highway_1",
                                      "us_highway_2",
                                      "us_highway_3",
                                      "us_state_1",
                                      "us_state_2",
                                      "us_state_3",
                                      "washington-metro",
                                      "wiener-linien",
                                      "zoo_icon" }),
              names);

    {
        auto& sprite = *std::find_if(images.begin(), images.end(), [] (const auto& image) { return image->getID() == "generic-metro"; });
        EXPECT_EQ(18u, sprite->getImage().size.width);
        EXPECT_EQ(18u, sprite->getImage().size.height);
        EXPECT_EQ(1, sprite->getPixelRatio());
        EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteparsing.png"), sprite->getImage());
    }
}

TEST(Sprite, SpriteParsingInvalidJSON) {
    const auto image_1x = util::read_file("test/fixtures/annotations/emerald.png");
    const auto json_1x = R"JSON({ "image": " })JSON";

    try {
        parseSprite(image_1x, json_1x);
        FAIL() << "Expected exception";
    } catch (std::runtime_error& err) {
        EXPECT_STREQ(
            "Failed to parse JSON: Missing a closing quotation mark in string. at offset 14",
            err.what());
    }
}

TEST(Sprite, SpriteParsingEmptyImage) {
    FixtureLog log;

    const auto image_1x = util::read_file("test/fixtures/annotations/emerald.png");
    const auto json_1x = R"JSON({ "image": {} })JSON";

    const auto images = parseSprite(image_1x, json_1x);
    EXPECT_EQ(0u, images.size());

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 0x0@0,0 in 200x299@1x sprite",
                  }));
}

TEST(Sprite, SpriteParsingSimpleWidthHeight) {
    FixtureLog log;

    const auto image_1x = util::read_file("test/fixtures/annotations/emerald.png");
    const auto json_1x = R"JSON({ "image": { "width": 32, "height": 32 } })JSON";

    const auto images = parseSprite(image_1x, json_1x);
    EXPECT_EQ(1u, images.size());
}

TEST(Sprite, SpriteParsingWidthTooBig) {
    FixtureLog log;

    const auto image_1x = util::read_file("test/fixtures/annotations/emerald.png");
    const auto json_1x = R"JSON({ "image": { "width": 65536, "height": 32 } })JSON";

    const auto images = parseSprite(image_1x, json_1x);
    EXPECT_EQ(0u, images.size());

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Warning,
                      Event::Sprite,
                      int64_t(-1),
                      "Value of 'width' must be an integer between 0 and 65535",
                  }));
    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 0x32@0,0 in 200x299@1x sprite",
                  }));
}

TEST(Sprite, SpriteParsingNegativeWidth) {
    FixtureLog log;

    const auto image_1x = util::read_file("test/fixtures/annotations/emerald.png");
    const auto json_1x = R"JSON({ "image": { "width": -1, "height": 32 } })JSON";

    const auto images = parseSprite(image_1x, json_1x);
    EXPECT_EQ(0u, images.size());

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Warning,
                      Event::Sprite,
                      int64_t(-1),
                      "Value of 'width' must be an integer between 0 and 65535",
                  }));
    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 0x32@0,0 in 200x299@1x sprite",
                  }));
}

TEST(Sprite, SpriteParsingNullRatio) {
    FixtureLog log;

    const auto image_1x = util::read_file("test/fixtures/annotations/emerald.png");
    const auto json_1x = R"JSON({ "image": { "width": 32, "height": 32, "pixelRatio": 0 } })JSON";

    const auto images = parseSprite(image_1x, json_1x);
    EXPECT_EQ(0u, images.size());

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Error,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics: 32x32@0,0 in 200x299@0x sprite",
                  }));
}
