#include "gtest/gtest.h"

#include <mbgl/map/map.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/std.hpp>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>

#include "./fixtures/fixture_log.hpp"

#include <dirent.h>
#include <signal.h>
#include <libgen.h>

std::string base_directory;


class ServerEnvironment : public ::testing::Environment {
public:
    virtual void SetUp() {
        pid = fork();
        if (pid < 0) {
            throw std::runtime_error("Cannot create web server");
        } else if (pid == 0) {
            char *arg[] = { nullptr };
            int ret = execv((base_directory + "bin/server.py").c_str(), arg);
            // This call should not return. In case execve failed, we exit anyway.
            if (ret < 0) {
                fprintf(stderr, "Failed to start server: %s\n", strerror(errno));
            }
            exit(0);
        } else {
            display = std::make_shared<mbgl::HeadlessDisplay>();
        }
    }
    virtual void TearDown() {
        ASSERT_TRUE(pid);
        kill(pid, SIGHUP);
    }

    std::shared_ptr<mbgl::HeadlessDisplay> display;

private:
    pid_t pid = 0;
};


ServerEnvironment* env = nullptr;


GTEST_API_ int main(int argc, char *argv[]) {
    base_directory = std::string(dirname(argv[0])) + "/" + dirname(const_cast<char *>(__FILE__)) + "/suite/";
    testing::InitGoogleTest(&argc, argv);
    env = new ServerEnvironment();
    ::testing::AddGlobalTestEnvironment(env);
    return RUN_ALL_TESTS();
}

class HeadlessTest : public ::testing::TestWithParam<std::string> {};

TEST_P(HeadlessTest, render) {
    using namespace mbgl;

    const std::string& base = GetParam();

    std::string style = util::read_file(base_directory + "tests/" + base + "/style.json");
    std::string info = util::read_file(base_directory + "tests/" + base + "/info.json");

    // Parse style.
    rapidjson::Document styleDoc;
    styleDoc.Parse<0>((const char *const)style.c_str());
    ASSERT_FALSE(styleDoc.HasParseError());
    ASSERT_TRUE(styleDoc.IsObject());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    styleDoc.Accept(writer);
    style = buffer.GetString();

    // Parse settings.
    rapidjson::Document infoDoc;
    infoDoc.Parse<0>((const char *const)info.c_str());
    ASSERT_FALSE(infoDoc.HasParseError());
    ASSERT_TRUE(infoDoc.IsObject());

    Log::Set<FixtureLogBackend>();

    for (auto it = infoDoc.MemberBegin(), end = infoDoc.MemberEnd(); it != end; it++) {
        const std::string name {
            it->name.GetString(), it->name.GetStringLength()
        };
        const rapidjson::Value& value = it->value;
        ASSERT_TRUE(value.IsObject());
        if (value.HasMember("center")) ASSERT_TRUE(value["center"].IsArray());

        const std::string actual_image = base_directory + "tests/" + base + "/" + name +  "/actual.png";

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

        HeadlessView view(env->display);
        Map map(view);

        map.setStyleJSON(style, base_directory);
        map.setAppliedClasses(classes);

        view.resize(width, height, pixelRatio);
        map.resize(width, height, pixelRatio);
        map.setLonLatZoom(longitude, latitude, zoom);
        map.setBearing(bearing);


        // Run the loop. It will terminate when we don't have any further listeners.
        map.run();

        const unsigned int w = width * pixelRatio;
        const unsigned int h = height * pixelRatio;

        auto pixels = view.readPixels();

        const int stride = w * 4;
        auto tmp = std::unique_ptr<char[]>(new char[stride]());
        char *rgba = reinterpret_cast<char *>(pixels.get());
        for (int i = 0, j = height - 1; i < j; i++, j--) {
            memcpy(tmp.get(), rgba + i * stride, stride);
            memcpy(rgba + i * stride, rgba + j * stride, stride);
            memcpy(rgba + j * stride, tmp.get(), stride);
        }

        const std::string image = util::compress_png(w, h, pixels.get());
        util::write_file(actual_image, image);
    }
}

INSTANTIATE_TEST_CASE_P(Headless, HeadlessTest, ::testing::ValuesIn([] {
    std::vector<std::string> names;

    DIR *dir = opendir((base_directory + "tests").c_str());
    if (dir == nullptr) {
        return names;
    }

    for (dirent *dp = nullptr; (dp = readdir(dir)) != nullptr;) {
        const std::string name = dp->d_name;
        if (name != "." && name != ".." && name != "index.html") {
            names.push_back(name);
        }
    }

    closedir(dir);

    return names;
}()));
