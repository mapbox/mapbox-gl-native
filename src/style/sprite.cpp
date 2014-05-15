#include <llmr/style/sprite.hpp>
#include <llmr/map/map.hpp>
#include <llmr/util/raster.hpp>

#include <string>
#include <llmr/platform/platform.hpp>
#include <llmr/util/uv.hpp>

#include <rapidjson/document.h>

using namespace llmr;

SpritePosition::SpritePosition(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t pixelRatio)
    : x(x),
      y(y),
      width(width),
      height(height),
      pixelRatio(pixelRatio) {
}

ImagePosition::ImagePosition(const vec2<uint16_t>& size, vec2<float> tl, vec2<float> br)
    : size(size),
      tl(tl),
      br(br) {}

Sprite::Sprite(Map &map, float pixelRatio)
    : pixelRatio(pixelRatio),
      raster(map.getTexturepool()),
      map(map),
      loaded(false) {
}

void Sprite::load(const std::string& base_url) {
   std::shared_ptr<Sprite> sprite = shared_from_this();

   std::string suffix = (pixelRatio > 1 ? "@2x" : "");

   platform::request_http(base_url + suffix + ".json", [sprite](platform::Response *res) {
       if (res->code == 200) {
           sprite->body.swap(res->body);
           sprite->asyncParseJSON();
       } else {
           fprintf(stderr, "failed to load sprite\n");
       }
   }, map.getLoop());

   platform::request_http(base_url + suffix + ".png", [sprite](platform::Response *res) {
       if (res->code == 200) {
            sprite->image.swap(res->body);
            sprite->asyncParseImage();
       } else {
           fprintf(stderr, "failed to load sprite image\n");
       }
   }, map.getLoop());
}

void Sprite::complete(std::shared_ptr<Sprite> &sprite) {
   if (sprite->raster.isLoaded() && sprite->pos.size()) {
       sprite->loaded = true;
       sprite->map.update();
       fprintf(stderr, "sprite loaded\n");
   }
}

bool Sprite::isLoaded() const {
    return loaded;
}

void Sprite::asyncParseImage() {
    new uv::work<std::shared_ptr<Sprite>>(map.getLoop(), parseImage, complete, shared_from_this());
}

void Sprite::asyncParseJSON() {
    new uv::work<std::shared_ptr<Sprite>>(map.getLoop(), parseJSON, complete, shared_from_this());
}

void Sprite::parseImage(std::shared_ptr<Sprite> &sprite) {
    sprite->raster.load(sprite->image);
    sprite->image.clear();
}

void Sprite::parseJSON(std::shared_ptr<Sprite> &sprite) {
    rapidjson::Document d;
    d.Parse<0>(sprite->body.c_str());
    sprite->body.clear();

    if (d.IsObject()) {
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
                sprite->pos.emplace(name, SpritePosition { x, y, width, height, pixelRatio });
            }
        }
    }
}

ImagePosition Sprite::getPosition(const std::string& name, bool repeating) {
    if (!isLoaded()) return ImagePosition {};

    // `repeating` indicates that the image will be used in a repeating pattern
    // repeating pattern images are assumed to have a 1px padding that mirrors the opposite edge
    // positions for repeating images are adjusted to exclude the edge
    int8_t offset = repeating ? 1 : 0;

    auto it = pos.find(name);
    if (it == pos.end()) return ImagePosition {};

    SpritePosition& pos = it->second;
    return ImagePosition {
        {
            pos.width,
            pos.height
        },
        {
            (float)(pos.x + offset) / raster.width,
            (float)(pos.y + offset) / raster.height
        },
        {
            (float)(pos.x + pos.width - 2 * offset) / raster.width,
            (float)(pos.y + pos.height - 2 * offset) / raster.height
        }
    };
}
