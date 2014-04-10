#include <llmr/style/sprite.hpp>
#include <llmr/util/raster.hpp>

#include <string>
#include <llmr/platform/platform.hpp>

#include <rapidjson/document.h>

using namespace llmr;

SpritePosition::SpritePosition(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t pixelRatio)
    : x(x),
      y(y),
      width(width),
      height(height),
      pixelRatio(pixelRatio) {}

ImagePosition::ImagePosition(const vec2<uint16_t>& size, vec2<float> tl, vec2<float> br)
    : size(size),
      tl(tl),
      br(br) {}

Sprite::Sprite(float pixelRatio) : pixelRatio(pixelRatio), raster(std::make_shared<Raster>()) {}

void Sprite::load(const std::string& base_url) {
    std::shared_ptr<Sprite> sprite = shared_from_this();

    auto complete = [sprite]() {
        std::lock_guard<std::mutex> lock(sprite->mtx);
        if (*sprite->raster && sprite->pos.size()) {
            sprite->loaded = true;
            platform::restart();
            fprintf(stderr, "sprite loaded\n");
        }
    };

    std::string suffix = (pixelRatio > 1 ? "@2x" : "");

    platform::request_http(base_url + suffix + ".json", [sprite](const platform::Response & res) {
        if (res.code == 200) {
            sprite->parseJSON(res.body);
        } else {
            fprintf(stderr, "failed to load sprite\n");
        }
    }, complete);

    platform::request_http(base_url + suffix + ".png", [sprite](const platform::Response & res) {
        if (res.code == 200) {
            sprite->raster->load(res.body);
        } else {
            fprintf(stderr, "failed to load sprite image\n");
        }
    }, complete);
}

Sprite::operator bool() const {
    std::lock_guard<std::mutex> lock(mtx);
    return loaded;
}

void Sprite::parseJSON(const std::string& data) {
    std::lock_guard<std::mutex> lock(mtx);

    rapidjson::Document d;
    d.Parse<0>(data.c_str());

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
                if (value.HasMember("pixelRatio")) pixelRatio = value["height"].GetInt();

                pos.insert({ name, { x, y, width, height, pixelRatio } });
            }
        }
    }
}

ImagePosition Sprite::getPosition(const std::string& name, bool repeating) {
    if (!*this->raster) return {};

    // `repeating` indicates that the image will be used in a repeating pattern
    // repeating pattern images are assumed to have a 1px padding that mirrors the opposite edge
    // positions for repeating images are adjusted to exclude the edge
    int8_t offset = repeating ? 1 : 0;

    auto it = pos.find(name);
    if (it == pos.end()) return {};

    SpritePosition& pos = it->second;
    return {
        {
            pos.width,
            pos.height
        },
        {
            (float)(pos.x + offset) / raster->width,
            (float)(pos.y + offset) / raster->height
        },
        {
            (float)(pos.x + pos.width - 2 * offset) / raster->width,
            (float)(pos.y + pos.height - 2 * offset) / raster->height
        }
    };
}
