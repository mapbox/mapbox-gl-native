#pragma once

#include <mbgl/style/image.hpp>

#include <string>
#include <map>
#include <set>
#include <vector>
#include <array>
#include <memory>

namespace mbgl {

class FileSource;
class SpriteLoaderObserver;

class SpriteLoader {
public:
    SpriteLoader(float pixelRatio);
    ~SpriteLoader();

    void load(const std::string& url, FileSource&);

    void setObserver(SpriteLoaderObserver*);

private:
    void emitSpriteLoadedIfComplete();

    // Invoked by SpriteAtlasWorker
    friend class SpriteLoaderWorker;
    void onParsed(std::vector<Immutable<style::Image::Impl>>);
    void onError(std::exception_ptr);

    const float pixelRatio;

    struct Loader;
    std::unique_ptr<Loader> loader;

    SpriteLoaderObserver* observer = nullptr;
};

} // namespace mbgl
