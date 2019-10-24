#pragma once

#include <mbgl/style/image_impl.hpp>
#include <mbgl/util/immutable.hpp>

#include <map>
#include <string>

namespace mbgl {

template <class T>
class Actor;

namespace gfx {
class UploadPass;
} // namespace gfx

class ImageManagerObserver;
class ImageRequestor;

/**
 * @brief tracks requests for icon images from tile workers and sends responses when the requests are fulfilled.
 */
class ImageManager {
public:
    ImageManager();
    ImageManager(const ImageManager&) = delete;
    ImageManager& operator=(const ImageManager&) = delete;
    ~ImageManager();

    void setObserver(ImageManagerObserver*);

    void setLoaded(bool);
    bool isLoaded() const;

    void dumpDebugLogs() const;

    const style::Image::Impl* getImage(const std::string&) const;
    const Immutable<style::Image::Impl>* getSharedImage(const std::string&) const;

    void addImage(Immutable<style::Image::Impl>);
    bool updateImage(Immutable<style::Image::Impl>);
    void removeImage(const std::string&);

    void getImages(ImageRequestor&, ImageRequestPair&&);
    void removeRequestor(ImageRequestor&);
    void notifyIfMissingImageAdded();
    void reduceMemoryUse();
    void reduceMemoryUseIfCacheSizeExceedsLimit();

    ImageVersionMap updatedImageVersions;

private:
    void checkMissingAndNotify(ImageRequestor&, const ImageRequestPair&);
    void notify(ImageRequestor&, const ImageRequestPair&) const;
    void removePattern(const std::string&);

    bool loaded = false;

    std::map<ImageRequestor*, ImageRequestPair> requestors;
    using Callback = std::function<void()>;
    using ActorCallback = Actor<Callback>;
    struct MissingImageRequestPair {
        ImageRequestPair pair;
        std::map<std::string, std::unique_ptr<ActorCallback>> callbacks;
    };
    std::map<ImageRequestor*, MissingImageRequestPair> missingImageRequestors;
    std::map<std::string, std::set<ImageRequestor*>> requestedImages;
    std::size_t requestedImagesCacheSize = 0ul;
    ImageMap images;

    ImageManagerObserver* observer = nullptr;
};

class ImageRequestor {
public:
    explicit ImageRequestor(ImageManager&);
    virtual ~ImageRequestor();
    virtual void onImagesAvailable(ImageMap icons, ImageMap patterns, ImageVersionMap versionMap, uint64_t imageCorrelationID) = 0;
private:
    ImageManager& imageManager;
};

} // namespace mbgl
