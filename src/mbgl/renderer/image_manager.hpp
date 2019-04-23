#pragma once

#include <mbgl/style/image_impl.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/gfx/texture.hpp>
#include <mbgl/renderer/image_manager_observer.hpp>

#include <mapbox/shelf-pack.hpp>

#include <set>
#include <string>

namespace mbgl {

namespace gfx {
class Context;
} // namespace gfx

class ImageRequestor {
public:
    virtual ~ImageRequestor() = default;
    virtual void onImagesAvailable(ImageMap icons, ImageMap patterns, ImageVersionMap versionMap, uint64_t imageCorrelationID) = 0;
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

    void setObserver(ImageManagerObserver*);

    void setLoaded(bool);
    bool isLoaded() const;

    void dumpDebugLogs() const;

    const style::Image::Impl* getImage(const std::string&) const;

    void addImage(Immutable<style::Image::Impl>);
    bool updateImage(Immutable<style::Image::Impl>);
    void removeImage(const std::string&);

    void getImages(ImageRequestor&, ImageRequestPair&&);
    void removeRequestor(ImageRequestor&);
    void notifyIfMissingImageAdded();
    void reduceMemoryUse();

    ImageVersionMap updatedImageVersions;

private:
    void checkMissingAndNotify(ImageRequestor&, const ImageRequestPair&);
    void notify(ImageRequestor&, const ImageRequestPair&) const;
    void removePattern(const std::string&);

    bool loaded = false;

    std::map<ImageRequestor*, ImageRequestPair> requestors;
    struct MissingImageRequestPair {
        ImageRequestPair pair;
        unsigned int callbacksRemaining;
    };
    std::map<ImageRequestor*, MissingImageRequestPair> missingImageRequestors;
    std::map<std::string, std::set<ImageRequestor*>> requestedImages;
    ImageMap images;

    ImageManagerObserver* observer = nullptr;

// Pattern stuff
public:
    optional<ImagePosition> getPattern(const std::string& name);

    gfx::TextureBinding textureBinding(gfx::Context&);
    void upload(gfx::Context&);

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
    mbgl::optional<gfx::Texture> atlasTexture;
    bool dirty = true;
};

} // namespace mbgl
