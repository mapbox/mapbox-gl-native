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

Sprite::Sprite(Map &map, float pixelRatio)
    : pixelRatio(pixelRatio),
      raster(),
      map(map),
      loadedImage(false),
      loadedJSON(false) {
}

void Sprite::load(const std::string& base_url) {
    loadedImage = false;
    loadedJSON = false;

    url = base_url;
    std::shared_ptr<Sprite> sprite = shared_from_this();

    std::string suffix = (pixelRatio > 1 ? "@2x" : "");

    platform::request_http(base_url + suffix + ".json", [sprite](platform::Response *res) {
        if (res->code == 200) {
            sprite->body.swap(res->body);
            sprite->asyncParseJSON();
        } else {
            fprintf(stderr, "failed to load sprite info\n");
            fprintf(stderr, "error %d: %s\n", res->code, res->error_message.c_str());
        }
    }, map.getLoop());

    platform::request_http(base_url + suffix + ".png", [sprite](platform::Response *res) {
        if (res->code == 200) {
             sprite->image.swap(res->body);
             sprite->asyncParseImage();
        } else {
            fprintf(stderr, "failed to load sprite image\n");
            fprintf(stderr, "error %d: %s\n", res->code, res->error_message.c_str());
        }
    }, map.getLoop());
}

void Sprite::complete(std::shared_ptr<Sprite> &sprite) {
    if (sprite->loadedImage && sprite->loadedJSON) {
        sprite->map.update();
        Log::Info(Event::Sprite, "loaded %s", sprite->url.c_str());
    }
}

bool Sprite::isLoaded() const {
    return loadedImage && loadedJSON;
}

void Sprite::asyncParseImage() {
    new uv::work<std::shared_ptr<Sprite>>(map.getLoop(), parseImage, complete, shared_from_this());
}

void Sprite::asyncParseJSON() {
    new uv::work<std::shared_ptr<Sprite>>(map.getLoop(), parseJSON, complete, shared_from_this());
}

void Sprite::parseImage(std::shared_ptr<Sprite> &sprite) {
    sprite->raster = std::make_unique<util::Image>(sprite->image);
    sprite->image.clear();
    sprite->loadedImage = true;
}

void Sprite::parseJSON(std::shared_ptr<Sprite> &sprite) {
    rapidjson::Document d;
    d.Parse<0>(sprite->body.c_str());
    sprite->body.clear();

    if (d.IsObject()) {
        std::unordered_map<std::string, SpritePosition> pos;

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

        sprite->pos.swap(pos);
        sprite->loadedJSON = true;
    }
}

const SpritePosition &Sprite::getSpritePosition(const std::string& name) const {
    if (!isLoaded()) return empty;
    auto it = pos.find(name);
    return it == pos.end() ? empty : it->second;
}
