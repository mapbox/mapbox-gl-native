#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/style/image.hpp>

#include <mbgl/util/logging.hpp>

#include <mbgl/util/image.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/string.hpp>

#include <cmath>
#include <limits>
#include <sstream>

namespace mbgl {

std::unique_ptr<style::Image> createStyleImage(const std::string& id,
                                               const PremultipliedImage& image,
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

    // Copy from the source image into our individual sprite image
    PremultipliedImage::copy(image, dstImage, { srcX, srcY }, { 0, 0 }, { width, height });

    return std::make_unique<style::Image>(id, std::move(dstImage), ratio, sdf);
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

std::vector<std::unique_ptr<style::Image>> parseSprite(const std::string& encodedImage, const std::string& json) {
    const PremultipliedImage raster = decodeImage(encodedImage);

    JSDocument doc;
    doc.Parse<0>(json.c_str());
    if (doc.HasParseError()) {
        std::stringstream message;
        message << "Failed to parse JSON: " << rapidjson::GetParseError_En(doc.GetParseError()) << " at offset " << doc.GetErrorOffset();
        throw std::runtime_error(message.str());
    } else if (!doc.IsObject()) {
        throw std::runtime_error("Sprite JSON root must be an object");
    } else {
        std::vector<std::unique_ptr<style::Image>> images;
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

                auto image = createStyleImage(name, raster, x, y, width, height, pixelRatio, sdf);
                if (image) {
                    images.push_back(std::move(image));
                }
            }
        }
        return images;
    }
}

} // namespace mbgl
