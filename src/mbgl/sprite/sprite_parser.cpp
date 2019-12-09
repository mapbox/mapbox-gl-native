#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/style/image.hpp>

#include <mbgl/util/exception.hpp>
#include <mbgl/util/logging.hpp>

#include <mbgl/util/image.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/string.hpp>

#include <cmath>
#include <limits>

namespace mbgl {

std::unique_ptr<style::Image> createStyleImage(const std::string& id,
                                               const PremultipliedImage& image,
                                               const uint32_t srcX,
                                               const uint32_t srcY,
                                               const uint32_t width,
                                               const uint32_t height,
                                               const double ratio,
                                               const bool sdf,
                                               style::ImageStretches&& stretchX,
                                               style::ImageStretches&& stretchY,
                                               optional<style::ImageContent> content) {
    // Disallow invalid parameter configurations.
    if (width <= 0 || height <= 0 || width > 1024 || height > 1024 ||
        ratio <= 0 || ratio > 10 ||
        srcX >= image.size.width || srcY >= image.size.height ||
        srcX + width > image.size.width || srcY + height > image.size.height) {
        Log::Error(Event::Sprite,
                   "Can't create image with invalid metrics: %ux%u@%u,%u in %ux%u@%sx sprite",
                   width,
                   height,
                   srcX,
                   srcY,
                   image.size.width,
                   image.size.height,
                   util::toString(ratio).c_str());
        return nullptr;
    }

    PremultipliedImage dstImage({ width, height });

    // Copy from the source image into our individual sprite image
    PremultipliedImage::copy(image, dstImage, { srcX, srcY }, { 0, 0 }, { width, height });

    try {
        return std::make_unique<style::Image>(
            id, std::move(dstImage), ratio, sdf, std::move(stretchX), std::move(stretchY), std::move(content));
    } catch (const util::StyleImageException& ex) {
        Log::Error(Event::Sprite, "Can't create image with invalid metadata: %s", ex.what());
        return nullptr;
    }
}

namespace {

uint16_t getUInt16(const JSValue& value, const char* property, const char* name, const uint16_t def = 0) {
    if (value.HasMember(property)) {
        auto& v = value[property];
        if (v.IsUint() && v.GetUint() <= std::numeric_limits<uint16_t>::max()) {
            return v.GetUint();
        } else {
            Log::Warning(Event::Sprite,
                         "Invalid sprite image '%s': value of '%s' must be an integer between 0 and 65535",
                         name,
                         property);
        }
    }

    return def;
}

double getDouble(const JSValue& value, const char* property, const char* name, const double def = 0) {
    if (value.HasMember(property)) {
        auto& v = value[property];
        if (v.IsNumber()) {
            return v.GetDouble();
        } else {
            Log::Warning(Event::Sprite, "Invalid sprite image '%s': value of '%s' must be a number", name, property);
        }
    }

    return def;
}

bool getBoolean(const JSValue& value, const char* property, const char* name, const bool def = false) {
    if (value.HasMember(property)) {
        auto& v = value[property];
        if (v.IsBool()) {
            return v.GetBool();
        } else {
            Log::Warning(Event::Sprite, "Invalid sprite image '%s': value of '%s' must be a boolean", name, property);
        }
    }

    return def;
}

style::ImageStretches getStretches(const JSValue& value, const char* property, const char* name) {
    style::ImageStretches stretches;

    if (value.HasMember(property)) {
        auto& v = value[property];
        if (v.IsArray()) {
            for (rapidjson::SizeType i = 0; i < v.Size(); ++i) {
                const JSValue& stretch = v[i];
                if (stretch.IsArray() && stretch.Size() == 2 && stretch[rapidjson::SizeType(0)].IsNumber() &&
                    stretch[rapidjson::SizeType(1)].IsNumber()) {
                    stretches.emplace_back(style::ImageStretch{stretch[rapidjson::SizeType(0)].GetFloat(),
                                                               stretch[rapidjson::SizeType(1)].GetFloat()});
                } else {
                    Log::Warning(Event::Sprite,
                                 "Invalid sprite image '%s': members of '%s' must be an array of two numbers",
                                 name,
                                 property);
                }
            }
        } else {
            Log::Warning(Event::Sprite, "Invalid sprite image '%s': value of '%s' must be an array", name, property);
        }
    }

    return stretches;
}

optional<style::ImageContent> getContent(const JSValue& value, const char* property, const char* name) {
    if (value.HasMember(property)) {
        auto& content = value[property];
        if (content.IsArray() && content.Size() == 4 && content[rapidjson::SizeType(0)].IsNumber() &&
            content[rapidjson::SizeType(1)].IsNumber() && content[rapidjson::SizeType(2)].IsNumber() &&
            content[rapidjson::SizeType(3)].IsNumber()) {
            return style::ImageContent{content[rapidjson::SizeType(0)].GetFloat(),
                                       content[rapidjson::SizeType(1)].GetFloat(),
                                       content[rapidjson::SizeType(2)].GetFloat(),
                                       content[rapidjson::SizeType(3)].GetFloat()};
        } else {
            Log::Warning(Event::Sprite,
                         "Invalid sprite image '%s': value of '%s' must be an array of four numbers",
                         name,
                         property);
        }
    }

    return nullopt;
}

} // namespace

std::vector<std::unique_ptr<style::Image>> parseSprite(const std::string& encodedImage, const std::string& json) {
    const PremultipliedImage raster = decodeImage(encodedImage);

    JSDocument doc;
    doc.Parse<0>(json.c_str());
    if (doc.HasParseError()) {
        throw std::runtime_error("Failed to parse JSON: " + formatJSONParseError(doc));
    } else if (!doc.IsObject()) {
        throw std::runtime_error("Sprite JSON root must be an object");
    } else {
        std::vector<std::unique_ptr<style::Image>> images;
        for (const auto& property : doc.GetObject()) {
            const std::string name = { property.name.GetString(), property.name.GetStringLength() };
            const JSValue& value = property.value;

            if (value.IsObject()) {
                const uint16_t x = getUInt16(value, "x", name.c_str(), 0);
                const uint16_t y = getUInt16(value, "y", name.c_str(), 0);
                const uint16_t width = getUInt16(value, "width", name.c_str(), 0);
                const uint16_t height = getUInt16(value, "height", name.c_str(), 0);
                const double pixelRatio = getDouble(value, "pixelRatio", name.c_str(), 1);
                const bool sdf = getBoolean(value, "sdf", name.c_str(), false);
                style::ImageStretches stretchX = getStretches(value, "stretchX", name.c_str());
                style::ImageStretches stretchY = getStretches(value, "stretchY", name.c_str());
                optional<style::ImageContent> content = getContent(value, "content", name.c_str());

                auto image = createStyleImage(name,
                                              raster,
                                              x,
                                              y,
                                              width,
                                              height,
                                              pixelRatio,
                                              sdf,
                                              std::move(stretchX),
                                              std::move(stretchY),
                                              std::move(content));
                if (image) {
                    images.push_back(std::move(image));
                }
            }
        }
        return images;
    }
}

} // namespace mbgl
