#include "../fixtures/util.hpp"
#include "../fixtures/fixture_log_observer.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/map/still_image.hpp>
#include <mbgl/util/image.hpp>

#include <mbgl/util/io.hpp>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <dirent.h>

#include <future>

void rewriteLocalScheme(rapidjson::Value &value, rapidjson::Document::AllocatorType &allocator) {
    ASSERT_TRUE(value.IsString());
    auto string = std::string { value.GetString(),value.GetStringLength() };
    if (string.compare(0, 8, "local://") == 0) {
        string.replace(0, 8, "http://127.0.0.1:2900/");
        value.SetString(string.data(), string.size(), allocator);
    }
}


class HeadlessTest : public ::testing::TestWithParam<std::string> {
public:
    static void SetUpTestCase() {
        const auto server = mbgl::platform::applicationRoot() + "/TEST_DATA/headless/server.js";
        pid = mbgl::test::startServer(server.c_str());
        display = std::make_shared<mbgl::HeadlessDisplay>();
    }

    static void TearDownTestCase() {
        display.reset();
        mbgl::test::stopServer(pid);
    }

protected:
    static pid_t pid;
    static std::shared_ptr<mbgl::HeadlessDisplay> display;
};

pid_t HeadlessTest::pid = 0;
std::shared_ptr<mbgl::HeadlessDisplay> HeadlessTest::display;

TEST_P(HeadlessTest, render) {
    using namespace mbgl;

    const std::string& base = GetParam();

    std::string style = util::read_file("test/suite/tests/" + base + "/style.json");
    std::string info = util::read_file("test/suite/tests/" + base + "/info.json");

    // Parse style.
    rapidjson::Document styleDoc;
    styleDoc.Parse<0>((const char *const)style.c_str());
    ASSERT_FALSE(styleDoc.HasParseError());
    ASSERT_TRUE(styleDoc.IsObject());

    // Rewrite "local://" to "http://127.0.0.1:2900/".
    if (styleDoc.HasMember("sprite")) {
        rewriteLocalScheme(styleDoc["sprite"], styleDoc.GetAllocator());
    }

    if (styleDoc.HasMember("glyphs")) {
        rewriteLocalScheme(styleDoc["glyphs"], styleDoc.GetAllocator());
    }

    if (styleDoc.HasMember("sources")) {
        auto &sources = styleDoc["sources"];
        ASSERT_TRUE(sources.IsObject());
        for (auto source = sources.MemberBegin(), end = sources.MemberEnd(); source != end; source++) {
            if (source->value.HasMember("tiles")) {
                auto &tiles = source->value["tiles"];
                ASSERT_TRUE(tiles.IsArray());
                for (rapidjson::SizeType i = 0; i < tiles.Size(); i++) {
                    rewriteLocalScheme(tiles[i], styleDoc.GetAllocator());
                }
            }

            if (source->value.HasMember("url") && source->value["url"].IsString()) {
                rewriteLocalScheme(source->value["url"], styleDoc.GetAllocator());
            }
        }
    }

    // Convert the JSON object back into a stringified version.
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    styleDoc.Accept(writer);
    style = buffer.GetString();

    // Parse settings.
    rapidjson::Document infoDoc;
    infoDoc.Parse<0>((const char *const)info.c_str());
    ASSERT_FALSE(infoDoc.HasParseError());
    ASSERT_TRUE(infoDoc.IsObject());

    for (auto it = infoDoc.MemberBegin(), end = infoDoc.MemberEnd(); it != end; it++) {
        const std::string name {
            it->name.GetString(), it->name.GetStringLength()
        };

        Log::Info(Event::General, "%s %s", base.c_str(), name.c_str());

        const rapidjson::Value& value = it->value;
        ASSERT_TRUE(value.IsObject());

        if (value.HasMember("native") && !value["native"].GetBool())
            continue;

        if (value.HasMember("center")) ASSERT_TRUE(value["center"].IsArray());

        const std::string actual_image = "test/suite/tests/" + base + "/" + name +  "/actual.png";

        const double zoom = value.HasMember("zoom") ? value["zoom"].GetDouble() : 0;
        const double bearing = value.HasMember("bearing") ? value["bearing"].GetDouble() : 0;
        const double latitude = value.HasMember("center") ? value["center"][rapidjson::SizeType(0)].GetDouble() : 0;
        const double longitude = value.HasMember("center") ? value["center"][rapidjson::SizeType(1)].GetDouble() : 0;
        const unsigned int width = value.HasMember("width") ? value["width"].GetUint() : 512;
        const unsigned int height = value.HasMember("height") ? value["height"].GetUint() : 512;
        const unsigned int pixelRatio = value.HasMember("pixelRatio") ? value["pixelRatio"].GetUint() : 1;

        std::vector<std::string> classes;
        if (value.HasMember("classes")) {
            const rapidjson::Value& js_classes = value["classes"];
            ASSERT_TRUE(js_classes.IsArray());
            for (rapidjson::SizeType i = 0; i < js_classes.Size(); i++) {
                const rapidjson::Value& js_class = js_classes[i];
                ASSERT_TRUE(js_class.IsString());
                classes.push_back({ js_class.GetString(), js_class.GetStringLength() });
            }
        }

        std::promise<void> promise;

        HeadlessView view(display, pixelRatio, width, height);
        DefaultFileSource fileSource(nullptr);
        Map map(view, fileSource, MapMode::Still);

        map.setClasses(classes);
        map.setStyleJSON(style, "test/suite");
        map.setLatLngZoom(mbgl::LatLng(latitude, longitude), zoom);
        map.setBearing(bearing);

        map.renderStill([&](std::exception_ptr error, std::unique_ptr<const StillImage> image) {
            if (error) {
                promise.set_exception(error);
                return;
            }
            const std::string png = util::compress_png(image->width, image->height, image->pixels.get());
            util::write_file("test/suite/tests/" + base + "/" + name +  "/actual.png", png);
            promise.set_value();
        });

        promise.get_future().get();
    }
}

INSTANTIATE_TEST_CASE_P(Headless, HeadlessTest, ::testing::ValuesIn([] {
    std::vector<std::string> names;

    const auto tests = mbgl::platform::applicationRoot() + "/TEST_DATA/suite/tests";
    DIR *dir = opendir(tests.c_str());
    if (dir != nullptr) {
        for (dirent *dp = nullptr; (dp = readdir(dir)) != nullptr;) {
            const std::string name = dp->d_name;
            if (name != "index.html" && !(name.size() >= 1 && name[0] == '.')) {
                names.push_back(name);
            }
        }
        closedir(dir);
    }

    EXPECT_GT(names.size(), 0ul);
    return names;
}()));
