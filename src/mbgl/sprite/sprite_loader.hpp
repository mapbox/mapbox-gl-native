#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/style/image.hpp>

#include <string>
#include <map>
#include <set>
#include <vector>
#include <array>
#include <memory>

namespace mbgl {

class Scheduler;
class FileSource;
class SpriteLoaderObserver;

class SpriteLoader : public util::noncopyable {
public:
    SpriteLoader(float pixelRatio);
    ~SpriteLoader();

    void load(const std::string& url, Scheduler&, FileSource&);

    bool isLoaded() const;

    void setObserver(SpriteLoaderObserver*);

private:
    void emitSpriteLoadedIfComplete();

    // Invoked by SpriteAtlasWorker
    friend class SpriteLoaderWorker;
    void onParsed(std::vector<std::unique_ptr<style::Image>>&&, uint64_t resultCorrelationID);
    void onError(std::exception_ptr, uint64_t resultCorrelationID);

    const float pixelRatio;

    struct Loader;
    std::unique_ptr<Loader> loader;

    SpriteLoaderObserver* observer = nullptr;
};

} // namespace mbgl
