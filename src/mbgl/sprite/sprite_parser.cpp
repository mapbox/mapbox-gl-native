#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/sprite/sprite_image.hpp>

#include <mbgl/platform/log.hpp>

#include <mbgl/util/image.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <cmath>
#include <limits>
#include <sstream>

namespace mbgl {

SpriteImagePtr createSpriteImage(const PremultipliedImage& image,
                                 const uint16_t srcX,
                                 const uint16_t srcY,
                                 const uint16_t srcWidth,
                                 const uint16_t srcHeight,
                                 const double ratio,
                                 const bool sdf) {
    // Disallow invalid parameter configurations.
    if (srcWidth == 0 || srcHeight == 0 || ratio <= 0 || ratio > 10 || srcWidth > 1024 ||
        srcHeight > 1024) {
        Log::Warning(Event::Sprite, "Can't create sprite with invalid metrics");
        return nullptr;
    }

    const uint16_t width = std::ceil(double(srcWidth) / ratio);
    const uint16_t dstWidth = std::ceil(width * ratio);
    assert(dstWidth >= srcWidth);
    const uint16_t height = std::ceil(double(srcHeight) / ratio);
    const uint16_t dstHeight = std::ceil(height * ratio);
    assert(dstHeight >= srcHeight);

    std::string data(dstWidth * dstHeight * 4, '\0');

    auto srcData = reinterpret_cast<const uint32_t*>(image.data.get());
    auto dstData = reinterpret_cast<uint32_t*>(const_cast<char*>(data.data()));

    const int32_t maxX = std::min(uint32_t(image.width), uint32_t(srcWidth + srcX)) - srcX;
    assert(maxX <= int32_t(image.width));
    const int32_t maxY = std::min(uint32_t(image.height), uint32_t(srcHeight + srcY)) - srcY;
    assert(maxY <= int32_t(image.height));

    // Copy from the source image into our individual sprite image
    for (uint16_t y = 0; y < maxY; ++y) {
        const auto dstRow = y * dstWidth;
        const auto srcRow = (y + srcY) * image.width + srcX;
        for (uint16_t x = 0; x < maxX; ++x) {
            dstData[dstRow + x] = srcData[srcRow + x];
        }
    }

    return std::make_unique<const SpriteImage>(width, height, ratio, std::move(data), sdf);
}

namespace {

inline uint16_t getUInt16(const rapidjson::Value& value, const char* name, const uint16_t def = 0) {
    if (value.HasMember(name)) {
        auto& v = value[name];
        if (v.IsUint() && v.GetUint() <= std::numeric_limits<uint16_t>::max()) {
            return v.GetUint();
        } else {
            Log::Warning(Event::Sprite, "Value of '%s' must be an integer between 0 and 65535",
                         name);
        }
    }

    return def;
}

inline double getDouble(const rapidjson::Value& value, const char* name, const double def = 0) {
    if (value.HasMember(name)) {
        auto& v = value[name];
        if (v.IsNumber()) {
            return v.GetDouble();
        } else {
            Log::Warning(Event::Sprite, "Value of '%s' must be a number", name);
        }
    }

    return def;
}

inline bool getBoolean(const rapidjson::Value& value, const char* name, const bool def = false) {
    if (value.HasMember(name)) {
        auto& v = value[name];
        if (v.IsBool()) {
            return v.GetBool();
        } else {
            Log::Warning(Event::Sprite, "Value of '%s' must be a boolean", name);
        }
    }

    return def;
}

} // namespace

SpriteParseResult parseSprite(const std::string& image, const std::string& json) {
    using namespace rapidjson;

    Sprites sprites;
    PremultipliedImage raster;

    try {
        raster = decodeImage(image);
    } catch (...) {
        return std::string("Could not parse sprite image");
    }

    Document doc;
    doc.Parse<0>(json.c_str());

    if (doc.HasParseError()) {
        std::stringstream message;
        message << "Failed to parse JSON: " << rapidjson::GetParseError_En(doc.GetParseError()) << " at offset " << doc.GetErrorOffset();
        return message.str();
    } else if (!doc.IsObject()) {
        return std::string("Sprite JSON root must be an object");
    } else {
        for (Value::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr) {
            const std::string name = { itr->name.GetString(), itr->name.GetStringLength() };
            const Value& value = itr->value;

            if (value.IsObject()) {
                const uint16_t x = getUInt16(value, "x", 0);
                const uint16_t y = getUInt16(value, "y", 0);
                const uint16_t width = getUInt16(value, "width", 0);
                const uint16_t height = getUInt16(value, "height", 0);
                const double pixelRatio = getDouble(value, "pixelRatio", 1);
                const bool sdf = getBoolean(value, "sdf", false);

                auto sprite = createSpriteImage(raster, x, y, width, height, pixelRatio, sdf);
                if (sprite) {
                    sprites.emplace(name, sprite);
                }
            }
        }
    }

    return sprites;
}

} // namespace mbgl
