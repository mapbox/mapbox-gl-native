#pragma once

#include <mbgl/geometry/binpack.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/style/image.hpp>

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
    SpriteAtlasElement(Rect<uint16_t>, const style::Image::Impl&, Size size, float pixelRatio);

    Rect<uint16_t> pos;
    bool sdf;

    float relativePixelRatio;
    std::array<float, 2> size;
    std::array<float, 2> tl;
    std::array<float, 2> br;
    float width;
    float height;
};

using IconMap = std::unordered_map<std::string, SpriteAtlasElement>;
using IconDependencies = std::set<std::string>;

class IconRequestor {
public:
    virtual void onIconsAvailable(IconMap) = 0;
};

class SpriteAtlas : public util::noncopyable {
public:
    SpriteAtlas(Size, float pixelRatio);
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

    optional<SpriteAtlasElement> getIcon(const std::string& name);
    optional<SpriteAtlasElement> getPattern(const std::string& name);

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    void bind(bool linear, gl::Context&, gl::TextureUnit unit);

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload(gl::Context&, gl::TextureUnit unit);

    Size getSize() const { return size; }
    float getPixelRatio() const { return pixelRatio; }

    // Only for use in tests.
    const PremultipliedImage& getAtlasImage() const {
        return image;
    }

private:
    const Size size;
    const float pixelRatio;
    bool loaded = false;

    struct Entry {
        Immutable<style::Image::Impl> image;

        // One sprite image might be used as both an icon image and a pattern image. If so,
        // it must have two distinct entries in the texture. The one for the icon image has
        // a single pixel transparent border, and the one for the pattern image has a single
        // pixel border wrapped from the opposite side.
        optional<Rect<uint16_t>> iconRect;
        optional<Rect<uint16_t>> patternRect;
    };

    optional<SpriteAtlasElement> getImage(const std::string& name, optional<Rect<uint16_t>> Entry::*rect);
    void copy(const Entry&, optional<Rect<uint16_t>> Entry::*rect);
    
    IconMap buildIconMap();

    std::unordered_map<std::string, Entry> entries;
    BinPack<uint16_t> bin;
    PremultipliedImage image;
    mbgl::optional<gl::Texture> texture;
    bool dirty;
    
    std::set<IconRequestor*> requestors;
    IconMap icons;
};

} // namespace mbgl
