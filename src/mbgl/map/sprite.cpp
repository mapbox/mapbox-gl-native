#include <mbgl/map/sprite.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/util/raster.hpp>
#include <mbgl/platform/log.hpp>

#include <string>
#include <mbgl/platform/platform.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/std.hpp>

#include <rapidjson/document.h>

using namespace mbgl;

SpritePosition::SpritePosition(uint16_t x_, uint16_t y_, uint16_t width_, uint16_t height_, float pixelRatio_, bool sdf_)
    : x(x_),
      y(y_),
      width(width_),
      height(height_),
      pixelRatio(pixelRatio_),
      sdf(sdf_) {
}

util::ptr<Sprite> Sprite::Create(const std::string& base_url, float pixelRatio, FileSource& fileSource) {
    util::ptr<Sprite> sprite(std::make_shared<Sprite>(Key(), base_url, pixelRatio));
    sprite->load(fileSource);
    return sprite;
}

Sprite::Sprite(const Key &, const std::string& base_url, float pixelRatio_)
    : valid(base_url.length() > 0),
      pixelRatio(pixelRatio_),
      spriteURL(base_url + (pixelRatio_ > 1 ? "@2x" : "") + ".png"),
      jsonURL(base_url + (pixelRatio_ > 1 ? "@2x" : "") + ".json"),
      raster(),
      loadedImage(false),
      loadedJSON(false),
      future(promise.get_future()) {
}

void Sprite::waitUntilLoaded() const {
    future.wait();
}

Sprite::operator bool() const {
    return valid && isLoaded() && !pos.empty();
}


// Note: This is a separate function that must be called exactly once after creation
// The reason this isn't part of the constructor is that calling shared_from_this() in
// the constructor fails.
void Sprite::load(FileSource& fileSource) {

    if (!valid) {
        // Treat a non-existent sprite as a successfully loaded empty sprite.
        loadedImage = true;
        loadedJSON = true;
        promise.set_value();
        return;
    }

    util::ptr<Sprite> sprite = shared_from_this();

    fileSource.request({ Resource::Kind::JSON, jsonURL }, [sprite](const Response &res) {
        if (res.status == Response::Successful) {
            sprite->body = res.data;
            sprite->parseJSON();
            sprite->complete();
        } else {
            Log::Warning(Event::Sprite, "Failed to load sprite info: %s", res.message.c_str());
            if (!sprite->future.valid()) {
                sprite->promise.set_exception(std::make_exception_ptr(std::runtime_error(res.message)));
            }
        }
    });

    fileSource.request({ Resource::Kind::Image, spriteURL }, [sprite](const Response &res) {
        if (res.status == Response::Successful) {
            sprite->image = res.data;
            sprite->parseImage();
            sprite->complete();
        } else {
            Log::Warning(Event::Sprite, "Failed to load sprite image: %s", res.message.c_str());
            if (!sprite->future.valid()) {
                sprite->promise.set_exception(std::make_exception_ptr(std::runtime_error(res.message)));
            }
        }
    });
}

void Sprite::complete() {
    if (loadedImage && loadedJSON) {
        promise.set_value();
    }
}

bool Sprite::isLoaded() const {
    return loadedImage && loadedJSON;
}

void Sprite::parseImage() {
    raster = util::make_unique<util::Image>(image);
    if (!*raster) {
        raster.reset();
    }
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
                float spritePixelRatio = 1.0f;
                bool sdf = false;

                if (value.HasMember("x")) x = value["x"].GetInt();
                if (value.HasMember("y")) y = value["y"].GetInt();
                if (value.HasMember("width")) width = value["width"].GetInt();
                if (value.HasMember("height")) height = value["height"].GetInt();
                if (value.HasMember("pixelRatio")) spritePixelRatio = value["pixelRatio"].GetInt();
                if (value.HasMember("sdf")) sdf = value["sdf"].GetBool();
                pos.emplace(name, SpritePosition { x, y, width, height, spritePixelRatio, sdf });
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
