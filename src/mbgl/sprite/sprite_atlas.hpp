#pragma once

#include <mbgl/geometry/binpack.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/style/image.hpp>

#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <array>
#include <memory>

namespace mbgl {

class Scheduler;
class FileSource;
class SpriteAtlasObserver;

namespace gl {
class Context;
} // namespace gl

class SpriteAtlasElement {
public:
    SpriteAtlasElement(Rect<uint16_t>, const style::Image&, Size size, float pixelRatio);

    Rect<uint16_t> pos;
    bool sdf;

    float relativePixelRatio;
    std::array<float, 2> size;
    std::array<float, 2> tl;
    std::array<float, 2> br;
    float width;
    float height;
};

typedef std::map<std::string, SpriteAtlasElement> IconMap;
typedef std::set<std::string> IconDependencies;

class IconRequestor {
public:
    virtual ~IconRequestor() = default;
    virtual void onIconsAvailable(IconMap) = 0;
};

class SpriteAtlas : public util::noncopyable {
public:
    using Images = std::map<std::string, std::unique_ptr<style::Image>>;

    SpriteAtlas(Size, float pixelRatio);
    ~SpriteAtlas();

    void load(const std::string& url, Scheduler&, FileSource&);

    void markAsLoaded() {
        loaded = true;
    }

    bool isLoaded() const {
        return loaded;
    }

    void dumpDebugLogs() const;

    void setObserver(SpriteAtlasObserver*);

    const style::Image* getImage(const std::string&) const;
    void addImage(const std::string&, std::unique_ptr<style::Image>);
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
    void emitSpriteLoadedIfComplete();

    // Invoked by SpriteAtlasWorker
    friend class SpriteAtlasWorker;
    void onParsed(Images&& result);
    void onError(std::exception_ptr);

    const Size size;
    const float pixelRatio;

    struct Loader;
    std::unique_ptr<Loader> loader;

    bool loaded = false;

    SpriteAtlasObserver* observer = nullptr;

    struct Entry {
        std::unique_ptr<style::Image> image;

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
