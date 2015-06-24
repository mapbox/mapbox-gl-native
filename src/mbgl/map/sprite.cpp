#include <mbgl/map/sprite.hpp>

#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/raster.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/uv_detail.hpp>

#include <rapidjson/document.h>

#include <string>
#include <sstream>

using namespace mbgl;

SpritePosition::SpritePosition(uint16_t x_, uint16_t y_, uint16_t width_, uint16_t height_, float pixelRatio_, bool sdf_)
    : x(x_),
      y(y_),
      width(width_),
      height(height_),
      pixelRatio(pixelRatio_),
      sdf(sdf_) {
}

Sprite::Sprite(const std::string& baseUrl, float pixelRatio_)
    : pixelRatio(pixelRatio_ > 1 ? 2 : 1),
      raster(),
      loadedImage(false),
      loadedJSON(false) {
    if (baseUrl.empty()) {
        // Treat a non-existent sprite as a successfully loaded empty sprite.
        loadedImage = true;
        loadedJSON = true;
        return;
    }

    std::string spriteURL(baseUrl + (pixelRatio_ > 1 ? "@2x" : "") + ".png");
    std::string jsonURL(baseUrl + (pixelRatio_ > 1 ? "@2x" : "") + ".json");

    FileSource* fs = util::ThreadContext::getFileSource();
    jsonRequest = fs->request({ Resource::Kind::JSON, jsonURL }, util::RunLoop::current.get()->get(), [this, jsonURL](const Response &res) {
        jsonRequest = nullptr;
        if (res.status == Response::Successful) {
            body = res.data;
            parseJSON(jsonURL);
        } else {
            std::stringstream message;
            message <<  "Failed to load [" << jsonURL << "]: " << res.message;
            emitSpriteLoadingFailed(message.str());
            return;
        }
        emitSpriteLoadedIfComplete();
    });

    spriteRequest = fs->request({ Resource::Kind::Image, spriteURL }, util::RunLoop::current.get()->get(), [this, spriteURL](const Response &res) {
        spriteRequest = nullptr;
        if (res.status == Response::Successful) {
            image = res.data;
            parseImage(spriteURL);
        } else {
            std::stringstream message;
            message <<  "Failed to load [" << spriteURL << "]: " << res.message;
            emitSpriteLoadingFailed(message.str());
            return;
        }
        emitSpriteLoadedIfComplete();
    });
}

Sprite::~Sprite() {
    if (jsonRequest) {
        util::ThreadContext::getFileSource()->cancel(jsonRequest);
    }

    if (spriteRequest) {
        util::ThreadContext::getFileSource()->cancel(spriteRequest);
    }
}

void Sprite::emitSpriteLoadedIfComplete() {
    if (isLoaded() && observer) {
        observer->onSpriteLoaded();
    }
}

void Sprite::emitSpriteLoadingFailed(const std::string& message) {
    if (!observer) {
        return;
    }

    auto error = std::make_exception_ptr(util::SpriteLoadingException(message));
    observer->onSpriteLoadingFailed(error);
}

bool Sprite::isLoaded() const {
    return loadedImage && loadedJSON;
}

bool Sprite::hasPixelRatio(float ratio) const {
    return pixelRatio == (ratio > 1 ? 2 : 1);
}

void Sprite::parseImage(const std::string& spriteURL) {
    raster = std::make_unique<util::Image>(image);
    if (!*raster) {
        raster.reset();
        std::stringstream message;
        message <<  "Failed to parse [" << spriteURL << "]";
        emitSpriteLoadingFailed(message.str());
        return;
    }

    image.clear();
    loadedImage = true;
}

void Sprite::parseJSON(const std::string& jsonURL) {
    rapidjson::Document d;
    d.Parse<0>(body.c_str());
    body.clear();

    if (d.HasParseError()) {
        std::stringstream message;
        message <<  "Failed to parse [" << jsonURL << "]: " << d.GetErrorOffset() << " - " << d.GetParseError();
        emitSpriteLoadingFailed(message.str());
    } else if (d.IsObject()) {
        for (rapidjson::Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
            const std::string name = { itr->name.GetString(), itr->name.GetStringLength() };
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
        loadedJSON = true;
    } else {
        std::stringstream message;
        message <<  "Failed to parse [" << jsonURL << "]: Root is not an object";
        emitSpriteLoadingFailed(message.str());
    }
}

const SpritePosition &Sprite::getSpritePosition(const std::string& name) const {
    if (!isLoaded()) return empty;
    auto it = pos.find(name);
    return it == pos.end() ? empty : it->second;
}

void Sprite::setObserver(Observer* observer_) {
    observer = observer_;
}
