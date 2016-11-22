#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/sprite/sprite_image.hpp>

#include <mbgl/util/logging.hpp>

#include <mbgl/util/image.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/string.hpp>

#include <cmath>
#include <limits>
#include <sstream>

namespace mbgl {

SpriteImagePtr createSpriteImage(const PremultipliedImage& image,
                                 const uint32_t srcX,
                                 const uint32_t srcY,
                                 const uint32_t width,
                                 const uint32_t height,
                                 const double ratio,
                                 const bool sdf) {
    // Disallow invalid parameter configurations.
    if (width <= 0 || height <= 0 || width > 1024 || height > 1024 ||
        ratio <= 0 || ratio > 10 ||
        srcX >= image.size.width || srcY >= image.size.height ||
        srcX + width > image.size.width || srcY + height > image.size.height) {
        Log::Error(Event::Sprite, "Can't create sprite with invalid metrics: %ux%u@%u,%u in %ux%u@%sx sprite",
            width, height, srcX, srcY,
            image.size.width, image.size.height,
            util::toString(ratio).c_str());
        return nullptr;
    }

    PremultipliedImage dstImage({ width, height });

    auto srcData = reinterpret_cast<const uint32_t*>(image.data.get());
    auto dstData = reinterpret_cast<uint32_t*>(dstImage.data.get());

    // Copy from the source image into our individual sprite image
    for (uint32_t y = 0; y < height; ++y) {
        const auto dstRow = y * width;
        const auto srcRow = (y + srcY) * image.size.width + srcX;
        for (uint32_t x = 0; x < width; ++x) {
            dstData[dstRow + x] = srcData[srcRow + x];
        }
    }

    return std::make_unique<const SpriteImage>(std::move(dstImage), ratio, sdf);
}

namespace {

uint16_t getUInt16(const JSValue& value, const char* name, const uint16_t def = 0) {
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

double getDouble(const JSValue& value, const char* name, const double def = 0) {
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

bool getBoolean(const JSValue& value, const char* name, const bool def = false) {
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
    Sprites sprites;
    PremultipliedImage raster;

    try {
        raster = decodeImage(image);
    } catch (...) {
        return std::current_exception();
    }

    JSDocument doc;
    doc.Parse<0>(json.c_str());

    if (doc.HasParseError()) {
        std::stringstream message;
        message << "Failed to parse JSON: " << rapidjson::GetParseError_En(doc.GetParseError()) << " at offset " << doc.GetErrorOffset();
        return std::make_exception_ptr(std::runtime_error(message.str()));
    } else if (!doc.IsObject()) {
        return std::make_exception_ptr(std::runtime_error("Sprite JSON root must be an object"));
    } else {
        for (const auto& property : doc.GetObject()) {
            const std::string name = { property.name.GetString(), property.name.GetStringLength() };
            const JSValue& value = property.value;

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
