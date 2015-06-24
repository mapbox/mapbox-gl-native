#include "../fixtures/util.hpp"
#include "../fixtures/fixture_log_observer.hpp"

#include <mbgl/annotation/sprite_parser.hpp>
#include <mbgl/annotation/sprite_image.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>

using namespace mbgl;

TEST(Annotations, SpriteImageCreationInvalid) {
    const util::Image image_1x = { util::read_file("test/fixtures/annotations/emerald.png") };
    ASSERT_TRUE(image_1x);
    ASSERT_EQ(200u, image_1x.getWidth());
    ASSERT_EQ(299u, image_1x.getHeight());

    // invalid dimensions
    ASSERT_EQ(nullptr, createSpriteImage(image_1x, 0, 0, 0, 16, 1, false));    // width == 0
    ASSERT_EQ(nullptr, createSpriteImage(image_1x, 0, 0, 16, 0, 1, false));    // height == 0
    ASSERT_EQ(nullptr, createSpriteImage(image_1x, 0, 0, 1, 1, 0, false));     // ratio == 0
    ASSERT_EQ(nullptr, createSpriteImage(image_1x, 0, 0, 1, 1, 23, false));    // ratio too large
    ASSERT_EQ(nullptr, createSpriteImage(image_1x, 0, 0, 2048, 16, 1, false)); // too wide
    ASSERT_EQ(nullptr, createSpriteImage(image_1x, 0, 0, 16, 1025, 1, false)); // too tall
}

TEST(Annotations, SpriteImageCreation1x) {
    const util::Image image_1x = { util::read_file("test/fixtures/annotations/emerald.png") };
    ASSERT_TRUE(image_1x);
    ASSERT_EQ(200u, image_1x.getWidth());
    ASSERT_EQ(299u, image_1x.getHeight());

    { // "museum_icon":{"x":177,"y":187,"width":18,"height":18,"pixelRatio":1,"sdf":false}
        const auto sprite = createSpriteImage(image_1x, 177, 187, 18, 18, 1, false);
        ASSERT_TRUE(sprite.get());
        EXPECT_EQ(18, sprite->width);
        EXPECT_EQ(18, sprite->height);
        EXPECT_EQ(18, sprite->pixelWidth);
        EXPECT_EQ(18, sprite->pixelHeight);
        EXPECT_EQ(1, sprite->pixelRatio);
        EXPECT_EQ(0xC83FE8FA9665D177u, std::hash<std::string>()(sprite->data));
    }

    { // outside image == blank
        const auto sprite = createSpriteImage(image_1x, 200, 0, 16, 16, 1, false);
        ASSERT_TRUE(sprite.get());
        EXPECT_EQ(16, sprite->width);
        EXPECT_EQ(16, sprite->height);
        EXPECT_EQ(16, sprite->pixelWidth);
        EXPECT_EQ(16, sprite->pixelHeight);
        EXPECT_EQ(1, sprite->pixelRatio);
        EXPECT_EQ(0x5599CFD89CB402A6u, std::hash<std::string>()(sprite->data));
    }

    { // outside image == blank
        const auto sprite = createSpriteImage(image_1x, 0, 300, 16, 16, 1, false);
        ASSERT_TRUE(sprite.get());
        EXPECT_EQ(16, sprite->width);
        EXPECT_EQ(16, sprite->height);
        EXPECT_EQ(16, sprite->pixelWidth);
        EXPECT_EQ(16, sprite->pixelHeight);
        EXPECT_EQ(1, sprite->pixelRatio);
        EXPECT_EQ(0x5599CFD89CB402A6u, std::hash<std::string>()(sprite->data));
    }
}

TEST(Annotations, SpriteImageCreation2x) {
    const util::Image image_2x = { util::read_file("test/fixtures/annotations/emerald@2x.png") };
    ASSERT_TRUE(image_2x);

    // "museum_icon":{"x":354,"y":374,"width":36,"height":36,"pixelRatio":2,"sdf":false}
    const auto sprite = createSpriteImage(image_2x, 354, 374, 36, 36, 2, false);
    ASSERT_TRUE(sprite.get());
    EXPECT_EQ(18, sprite->width);
    EXPECT_EQ(18, sprite->height);
    EXPECT_EQ(36, sprite->pixelWidth);
    EXPECT_EQ(36, sprite->pixelHeight);
    EXPECT_EQ(2, sprite->pixelRatio);
    EXPECT_EQ(0x2446A6D2C350B6AEu, std::hash<std::string>()(sprite->data));
}

TEST(Annotations, SpriteImageCreation1_5x) {
    const util::Image image_2x = { util::read_file("test/fixtures/annotations/emerald@2x.png") };
    ASSERT_TRUE(image_2x);

    // "museum_icon":{"x":354,"y":374,"width":36,"height":36,"pixelRatio":2,"sdf":false}
    const auto sprite = createSpriteImage(image_2x, 354, 374, 36, 36, 1.5, false);
    ASSERT_TRUE(sprite.get());
    EXPECT_EQ(24, sprite->width);
    EXPECT_EQ(24, sprite->height);
    EXPECT_EQ(36, sprite->pixelWidth);
    EXPECT_EQ(36, sprite->pixelHeight);
    EXPECT_EQ(1.5, sprite->pixelRatio);
    EXPECT_EQ(0x2446A6D2C350B6AEu, std::hash<std::string>()(sprite->data));

    // "hospital_icon":{"x":314,"y":518,"width":36,"height":36,"pixelRatio":2,"sdf":false}
    const auto sprite2 = createSpriteImage(image_2x, 314, 518, 35, 35, 1.5, false);
    ASSERT_TRUE(sprite2.get());
    EXPECT_EQ(24, sprite2->width);
    EXPECT_EQ(24, sprite2->height);
    EXPECT_EQ(36, sprite2->pixelWidth);
    EXPECT_EQ(36, sprite2->pixelHeight);
    EXPECT_EQ(1.5, sprite2->pixelRatio);
    EXPECT_EQ(0xF5274FF7FABA1C8Du, std::hash<std::string>()(sprite2->data));
}

TEST(Annotations, SpriteParsing) {
    const auto image_1x = util::read_file("test/fixtures/annotations/emerald.png");
    const auto json_1x = util::read_file("test/fixtures/annotations/emerald.json");

    const auto images = parseSprite(image_1x, json_1x);

    std::set<std::string> names;
    std::transform(images.begin(), images.end(), std::inserter(names, names.begin()),
                   [](const auto& pair) { return pair.first; });

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
        auto sprite = images.find("generic-metro")->second;
        EXPECT_EQ(18, sprite->width);
        EXPECT_EQ(18, sprite->height);
        EXPECT_EQ(18, sprite->pixelWidth);
        EXPECT_EQ(18, sprite->pixelHeight);
        EXPECT_EQ(1, sprite->pixelRatio);
        EXPECT_EQ(0X4829734034980451u, std::hash<std::string>()(sprite->data));
    }
}

TEST(Annotations, SpriteParsingInvalidJSON) {
    FixtureLog log;

    const auto image_1x = util::read_file("test/fixtures/annotations/emerald.png");
    const auto json_1x = R"JSON({ "image": " })JSON";

    const auto images = parseSprite(image_1x, json_1x);
    EXPECT_EQ(0u, images.size());

    EXPECT_EQ(
        1u,
        log.count({
            EventSeverity::Warning,
            Event::Sprite,
            int64_t(-1),
            "Failed to parse JSON: lacks ending quotation before the end of string at offset 10",
        }));
}

TEST(Annotations, SpriteParsingEmptyImage) {
    FixtureLog log;

    const auto image_1x = util::read_file("test/fixtures/annotations/emerald.png");
    const auto json_1x = R"JSON({ "image": {} })JSON";

    const auto images = parseSprite(image_1x, json_1x);
    EXPECT_EQ(0u, images.size());

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Warning,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics",
                  }));
}

TEST(Annotations, SpriteParsingSimpleWidthHeight) {
    FixtureLog log;

    const auto image_1x = util::read_file("test/fixtures/annotations/emerald.png");
    const auto json_1x = R"JSON({ "image": { "width": 32, "height": 32 } })JSON";

    const auto images = parseSprite(image_1x, json_1x);
    EXPECT_EQ(1u, images.size());
}

TEST(Annotations, SpriteParsingWidthTooBig) {
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
                      EventSeverity::Warning,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics",
                  }));
}

TEST(Annotations, SpriteParsingNegativeWidth) {
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
                      EventSeverity::Warning,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics",
                  }));
}

TEST(Annotations, SpriteParsingNullRatio) {
    FixtureLog log;

    const auto image_1x = util::read_file("test/fixtures/annotations/emerald.png");
    const auto json_1x = R"JSON({ "image": { "width": 32, "height": 32, "pixelRatio": 0 } })JSON";

    const auto images = parseSprite(image_1x, json_1x);
    EXPECT_EQ(0u, images.size());

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Warning,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't create sprite with invalid metrics",
                  }));
}
