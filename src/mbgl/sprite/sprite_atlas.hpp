#pragma once

#include <mbgl/gl/texture.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/rect.hpp>
#include <mbgl/style/image.hpp>

#include <mapbox/shelf-pack.hpp>

#include <string>
#include <set>
#include <unordered_map>
#include <array>
#include <memory>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

class SpriteAtlasElement {
public:
    SpriteAtlasElement(const mapbox::Bin&, const style::Image::Impl&);

    bool sdf;
    float pixelRatio;
    Rect<uint16_t> textureRect;

    std::array<uint16_t, 2> tl() const {
        return {{
            textureRect.x,
            textureRect.y
        }};
    }

    std::array<uint16_t, 2> br() const {
        return {{
            static_cast<uint16_t>(textureRect.x + textureRect.w),
            static_cast<uint16_t>(textureRect.y + textureRect.h)
        }};
    }

    std::array<float, 2> displaySize() const {
        return {{
            textureRect.w / pixelRatio,
            textureRect.h / pixelRatio,
        }};
    }
};

using IconMap = std::unordered_map<std::string, SpriteAtlasElement>;
using IconDependencies = std::set<std::string>;

class IconRequestor {
public:
    virtual ~IconRequestor() = default;
    virtual void onIconsAvailable(IconMap) = 0;
};

class SpriteAtlas : public util::noncopyable {
public:
    SpriteAtlas();
    ~SpriteAtlas();

    void onSpriteLoaded();

    void markAsLoaded() {
        loaded = true;
    }

    bool isLoaded() const {
        return loaded;
    }

    void dumpDebugLogs() const;

    const style::Image::Impl* getImage(const std::string&) const;
    void addImage(Immutable<style::Image::Impl>);
    void removeImage(const std::string&);

    void getIcons(IconRequestor& requestor);
    void removeRequestor(IconRequestor& requestor);

    // Ensure that the atlas contains the named image suitable for rendering as an icon, and
    // return its metrics. The image will be padded on each side with a one pixel wide transparent
    // strip, but the returned metrics are exclusive of this padding.
    optional<SpriteAtlasElement> getIcon(const std::string& name);

    // Ensure that the atlas contains the named image suitable for rendering as an pattern, and
    // return its metrics. The image will be padded on each side with a one pixel wide copy of
    // pixels from the opposite side, but the returned metrics are exclusive of this padding.
    optional<SpriteAtlasElement> getPattern(const std::string& name);

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    void bind(bool linear, gl::Context&, gl::TextureUnit unit);

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload(gl::Context&, gl::TextureUnit unit);

    Size getPixelSize() const;

    // Only for use in tests.
    const PremultipliedImage& getAtlasImage() const {
        return image;
    }

private:
    bool loaded = false;

    struct Entry {
        Immutable<style::Image::Impl> image;

        // One sprite image might be used as both an icon image and a pattern image. If so,
        // it must have two distinct entries in the texture. The one for the icon image has
        // a single pixel transparent border, and the one for the pattern image has a single
        // pixel border wrapped from the opposite side.
        mapbox::Bin* iconBin = nullptr;
        mapbox::Bin* patternBin = nullptr;
    };

    optional<SpriteAtlasElement> getImage(const std::string& name, mapbox::Bin* Entry::*bin);
    void copy(const Entry&, mapbox::Bin* Entry::*bin);
    
    IconMap buildIconMap();

    std::unordered_map<std::string, Entry> entries;
    mapbox::ShelfPack shelfPack;
    PremultipliedImage image;
    mbgl::optional<gl::Texture> texture;
    bool dirty = true;
    
    std::set<IconRequestor*> requestors;
    IconMap icons;
};

} // namespace mbgl
