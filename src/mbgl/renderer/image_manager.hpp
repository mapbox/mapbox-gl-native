#pragma once

#include <mbgl/style/image_impl.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/gl/texture.hpp>

#include <mapbox/shelf-pack.hpp>

#include <set>
#include <string>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

class ImageRequestor {
public:
    virtual ~ImageRequestor() = default;
    virtual void onImagesAvailable(ImageMap, uint64_t imageCorrelationID) = 0;
};

/*
    ImageManager does two things:

        1. Tracks requests for icon images from tile workers and sends responses when the requests are fulfilled.
        2. Builds a texture atlas for pattern images.

    These are disparate responsibilities and should eventually be handled by different classes. When we implement
    data-driven support for `*-pattern`, we'll likely use per-bucket pattern atlases, and that would be a good time
    to refactor this.
*/
class ImageManager : public util::noncopyable {
public:
    ImageManager();
    ~ImageManager();

    void setLoaded(bool);
    bool isLoaded() const;

    void dumpDebugLogs() const;

    const style::Image::Impl* getImage(const std::string&) const;

    void addImage(Immutable<style::Image::Impl>);
    void updateImage(Immutable<style::Image::Impl>);
    void removeImage(const std::string&);

    void getImages(ImageRequestor&, ImageRequestPair&&);
    void removeRequestor(ImageRequestor&);

private:
    void notify(ImageRequestor&, const ImageRequestPair&) const;

    bool loaded = false;

    std::unordered_map<ImageRequestor*, ImageRequestPair> requestors;
    ImageMap images;

// Pattern stuff
public:
    optional<ImagePosition> getPattern(const std::string& name);

    void bind(gl::Context&, gl::TextureUnit unit);
    void upload(gl::Context&, gl::TextureUnit unit);

    Size getPixelSize() const;

    // Only for use in tests.
    const PremultipliedImage& getAtlasImage() const {
        return atlasImage;
    }

private:
    struct Pattern {
        mapbox::Bin* bin;
        ImagePosition position;
    };

    mapbox::ShelfPack shelfPack;
    std::unordered_map<std::string, Pattern> patterns;
    PremultipliedImage atlasImage;
    mbgl::optional<gl::Texture> atlasTexture;
    bool dirty = true;
};

} // namespace mbgl
