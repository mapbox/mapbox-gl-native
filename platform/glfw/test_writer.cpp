#include "test_writer.hpp"

#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <ghc/filesystem.hpp>

#include <cmath>
#include <fstream>

using Writer = rapidjson::PrettyWriter<rapidjson::StringBuffer>;

class TestOperationSerializer {
public:
    virtual ~TestOperationSerializer() = default;

    virtual void serialize(Writer& writer) const = 0;
};

class SetCamera final : public TestOperationSerializer {
public:
    SetCamera(const mbgl::CameraOptions& camera_) : camera(camera_) {}

    void serialize(Writer& writer) const override {
        if (camera.zoom) {
            writer.StartArray();
            writer.String("setZoom");
            writer.Double(*camera.zoom);
            writer.EndArray();
        }

        if (camera.bearing && *camera.bearing != 0.) {
            writer.StartArray();
            writer.String("setBearing");
            writer.Double(*camera.bearing);
            writer.EndArray();
        }

        if (camera.pitch && *camera.pitch != 0.) {
            writer.StartArray();
            writer.String("setPitch");
            writer.Int(std::round(*camera.pitch));
            writer.EndArray();
        }

        writer.StartArray();
        writer.String("setCenter");
        writer.StartArray();
        writer.Double(camera.center->longitude());
        writer.Double(camera.center->latitude());
        writer.EndArray();
        writer.EndArray();
    }

private:
    mbgl::CameraOptions camera;
};

class SetStyle final : public TestOperationSerializer {
public:
    SetStyle(const mbgl::style::Style& style) : url(style.getURL()) {}

    void serialize(Writer& writer) const override {
        writer.StartArray();
        writer.String("setStyle");
        writer.String(url);
        writer.EndArray();
    }

private:
    std::string url;
};

class SetInitialSize final : public TestOperationSerializer {
public:
    SetInitialSize(const mbgl::Size& size) : width(size.width), height(size.height) {}

    void serialize(Writer& writer) const override {
        writer.Key("width");
        writer.Int(width);

        writer.Key("height");
        writer.Int(height);
    }

private:
    uint32_t width;
    uint32_t height;
};

TestWriter::TestWriter() = default;

TestWriter::~TestWriter() = default;

TestWriter& TestWriter::withCameraOptions(const mbgl::CameraOptions& camera) {
    operations.emplace_back(std::make_unique<SetCamera>(camera));

    return *this;
}

TestWriter& TestWriter::withStyle(const mbgl::style::Style& style) {
    operations.emplace_back(std::make_unique<SetStyle>(style));

    return *this;
}

TestWriter& TestWriter::withInitialSize(const mbgl::Size& size) {
    assert(initialSize == nullptr);
    initialSize = std::make_unique<SetInitialSize>(size);

    return *this;
}

bool TestWriter::write(const std::string& dir) const {
    namespace fs = ghc::filesystem;

    fs::path rootDir(dir);
    if (!fs::exists(rootDir)) {
        return false;
    }

    fs::path testDir;
    for (int i = 0; i < 1000; ++i) {
        std::string suffix = std::to_string(i);
        suffix.insert(suffix.begin(), 3 - suffix.length(), '0');

        testDir = rootDir / suffix;
        if (!fs::exists(testDir)) {
            break;
        }
    }

    if (!fs::create_directory(testDir)) {
        return false;
    }

    fs::path styleFile = testDir / "style.json";

    std::ofstream out;
    out.open(styleFile.string(), std::ios::out);
    out << serialize();

    return out.is_open() && out.good();
}

std::string TestWriter::serialize() const {
    rapidjson::StringBuffer s;
    Writer writer(s);

    writer.StartObject();

    writer.Key("version");
    writer.Int(8);

    writer.Key("metadata");
    writer.StartObject();

    writer.Key("test");
    writer.StartObject();

    writer.Key("operations");
    writer.StartArray();

    for (const auto& operation : operations) {
        operation->serialize(writer);
    }

    writer.EndArray();

    if (initialSize) {
        initialSize->serialize(writer);
    }

    writer.EndObject();
    writer.EndObject();

    writer.Key("sources");
    writer.StartObject();
    writer.EndObject();

    writer.Key("layers");
    writer.StartArray();
    writer.EndArray();

    writer.EndObject();

    return s.GetString();
}
