#include <mbgl/map/sprite.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/util/raster.hpp>
#include <mbgl/platform/log.hpp>

#include <string>
#include <mbgl/platform/platform.hpp>
#include <mbgl/util/uv.hpp>
#include <mbgl/util/std.hpp>

#include <rapidjson/document.h>

using namespace mbgl;

SpritePosition::SpritePosition(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t pixelRatio)
    : x(x),
      y(y),
      width(width),
      height(height),
      pixelRatio(pixelRatio) {
}

std::shared_ptr<Sprite> Sprite::Create(const std::string& base_url, float pixelRatio) {
    std::shared_ptr<Sprite> sprite(std::make_shared<Sprite>(Key(), base_url, pixelRatio));
    sprite->load();
    return sprite;
}

Sprite::Sprite(const Key &, const std::string& base_url, float pixelRatio)
    : pixelRatio(pixelRatio),
      url(base_url),
      raster(),
      loadedImage(false),
      loadedJSON(false),
      future(promise.get_future()) {
}

void Sprite::waitUntilLoaded() const {
    future.wait();
}

Sprite::operator bool() const {
    return isLoaded() && !pos.empty();
}


// Note: This is a separate function that must be called exactly once after creation
// The reason this isn't part of the constructor is that calling shared_from_this() in
// the constructor fails.
void Sprite::load() {
    if (url.length() == 0) {
        // Treat a non-existent sprite as an empty sprite.
        loadedImage = true;
        loadedJSON = true;
        promise.set_value();
        return;
    }

    std::shared_ptr<Sprite> sprite = shared_from_this();

    const std::string suffix = (pixelRatio > 1 ? "@2x" : "");

    platform::request_http(url + suffix + ".json", [sprite](platform::Response *res) {
        if (res->code == 200) {
            sprite->body.swap(res->body);
            sprite->parseJSON();
            sprite->complete();
        } else {
            fprintf(stderr, "failed to load sprite info\n");
            fprintf(stderr, "error %d: %s\n", res->code, res->error_message.c_str());
            sprite->promise.set_exception(std::make_exception_ptr(std::runtime_error(res->error_message)));
        }
    });

    platform::request_http(url + suffix + ".png", [sprite](platform::Response *res) {
        if (res->code == 200) {
            sprite->image.swap(res->body);
            sprite->parseImage();
            sprite->complete();
        } else {
            fprintf(stderr, "failed to load sprite image\n");
            fprintf(stderr, "error %d: %s\n", res->code, res->error_message.c_str());
            sprite->promise.set_exception(std::make_exception_ptr(std::runtime_error(res->error_message)));
        }
    });
}

void Sprite::complete() {
    if (loadedImage && loadedJSON) {
        Log::Info(Event::Sprite, "loaded %s", url.c_str());
        promise.set_value();
    }
}

bool Sprite::isLoaded() const {
    return loadedImage && loadedJSON;
}

void Sprite::parseImage() {
    raster = std::make_unique<util::Image>(image);
    image.clear();
    loadedImage = true;
}

void Sprite::parseJSON() {
    rapidjson::Document d;
    d.Parse<0>(body.c_str());
    body.clear();

    if (d.HasParseError()) {
        Log::Warning(Event::Sprite, "sprite JSON is invalid");
    } else if (d.IsObject()) {
        for (rapidjson::Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
            const std::string& name = itr->name.GetString();
            const rapidjson::Value& value = itr->value;

            if (value.IsObject()) {
                uint16_t x = 0;
                uint16_t y = 0;
                uint16_t width = 0;
                uint16_t height = 0;
                uint8_t pixelRatio = 1;

                if (value.HasMember("x")) x = value["x"].GetInt();
                if (value.HasMember("y")) y = value["y"].GetInt();
                if (value.HasMember("width")) width = value["width"].GetInt();
                if (value.HasMember("height")) height = value["height"].GetInt();
                if (value.HasMember("pixelRatio")) pixelRatio = value["pixelRatio"].GetInt();
                pos.emplace(name, SpritePosition { x, y, width, height, pixelRatio });
            }
        }
    } else {
        Log::Warning(Event::Sprite, "sprite JSON root is not an object");
    }

    loadedJSON = true;
}

const SpritePosition &Sprite::getSpritePosition(const std::string& name) const {
    if (!isLoaded()) return empty;
    auto it = pos.find(name);
    return it == pos.end() ? empty : it->second;
}
