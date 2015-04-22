#include <mbgl/map/sprite.hpp>
#include <mbgl/util/raster.hpp>
#include <mbgl/platform/log.hpp>

#include <string>
#include <mbgl/platform/platform.hpp>
#include <mbgl/map/environment.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
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

Sprite::Sprite(const std::string& baseUrl, float pixelRatio_, Environment& env, std::function<void ()> callback_)
    : pixelRatio(pixelRatio_ > 1 ? 2 : 1),
      raster(),
      loadedImage(false),
      loadedJSON(false),
      future(promise.get_future()),
      callback(callback_) {

    if (baseUrl.empty()) {
        // Treat a non-existent sprite as a successfully loaded empty sprite.
        loadedImage = true;
        loadedJSON = true;
        promise.set_value();
        return;
    }

    std::string spriteURL(baseUrl + (pixelRatio_ > 1 ? "@2x" : "") + ".png");
    std::string jsonURL(baseUrl + (pixelRatio_ > 1 ? "@2x" : "") + ".json");

    jsonRequest = env.request({ Resource::Kind::JSON, jsonURL }, [this](const Response &res) {
        jsonRequest = nullptr;
        if (res.status == Response::Successful) {
            body = res.data;
            parseJSON();
        } else {
            Log::Warning(Event::Sprite, "Failed to load sprite info: %s", res.message.c_str());
        }
        loadedJSON = true;
        complete();
    });

    spriteRequest = env.request({ Resource::Kind::Image, spriteURL }, [this](const Response &res) {
        spriteRequest = nullptr;
        if (res.status == Response::Successful) {
            image = res.data;
            parseImage();
        } else {
            Log::Warning(Event::Sprite, "Failed to load sprite image: %s", res.message.c_str());
        }
        loadedImage = true;
        complete();
    });
}

Sprite::~Sprite() {
    if (jsonRequest) {
        jsonRequest->cancel();
    }

    if (spriteRequest) {
        spriteRequest->cancel();
    }
}

void Sprite::complete() {
    if (loadedImage && loadedJSON) {
        promise.set_value();
        callback();
    }
}

bool Sprite::isLoaded() const {
    return loadedImage && loadedJSON;
}

void Sprite::waitUntilLoaded() const {
    future.wait();
}

bool Sprite::hasPixelRatio(float ratio) const {
    return pixelRatio == (ratio > 1 ? 2 : 1);
}

void Sprite::parseImage() {
    raster = util::make_unique<util::Image>(image);
    if (!*raster) {
        raster.reset();
    }
    image.clear();
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
}

const SpritePosition &Sprite::getSpritePosition(const std::string& name) const {
    if (!isLoaded()) return empty;
    auto it = pos.find(name);
    return it == pos.end() ? empty : it->second;
}
