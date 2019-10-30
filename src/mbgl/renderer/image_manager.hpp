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
    const std::set<std::string>& getAvailableImages() const;

    ImageVersionMap updatedImageVersions;

private:
    void checkMissingAndNotify(ImageRequestor&, const ImageRequestPair&);
    void notify(ImageRequestor&, const ImageRequestPair&) const;
    void removePattern(const std::string&);

    bool loaded = false;

    std::map<ImageRequestor*, ImageRequestPair> requestors;
    std::map<ImageRequestor*, ImageRequestPair> missingImageRequestors;
    std::map<std::string, std::set<ImageRequestor*>> requestedImages;
    std::size_t requestedImagesCacheSize = 0ul;
    ImageMap images;
    // Mirror of 'ImageMap images;' keys.
    std::set<std::string> availableImages;

    ImageManagerObserver* observer = nullptr;
};

class ImageRequestor {
public:
    explicit ImageRequestor(ImageManager&);
    virtual ~ImageRequestor();
    virtual void onImagesAvailable(ImageMap icons, ImageMap patterns, ImageVersionMap versionMap, uint64_t imageCorrelationID) = 0;

    void addPendingRequest(const std::string& imageId) { pendingRequests.insert(imageId); }
    bool hasPendingRequest(const std::string& imageId) const { return pendingRequests.count(imageId); }
    bool hasPendingRequests() const { return !pendingRequests.empty(); }
    void removePendingRequest(const std::string& imageId) { pendingRequests.erase(imageId); }

private:
    ImageManager& imageManager;

    // Pending requests are image requests that are waiting to be dispatched to the client.
    std::set<std::string> pendingRequests;
};

} // namespace mbgl
