#pragma once

#include <mbgl/actor/actor.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/style/image.hpp>

#include <string>
#include <map>
#include <set>
#include <vector>
#include <array>
#include <memory>

namespace mbgl {

class AsyncRequest;
class Scheduler;
class FileSource;
class SpriteLoaderObserver;

class SpriteLoader : public util::noncopyable {
public:
    SpriteLoader(ActorRef<SpriteLoader>, ActorRef<SpriteLoaderObserver>, FileSource&, float pixelRatio);
    ~SpriteLoader();

    void load(const std::string& url);

private:
    ActorRef<SpriteLoader> self;
    ActorRef<SpriteLoaderObserver> observer;
    FileSource& fileSource;
    const float pixelRatio;

    std::unique_ptr<AsyncRequest> jsonRequest;
    std::unique_ptr<AsyncRequest> spriteRequest;

    std::shared_ptr<const std::string> image;
    std::shared_ptr<const std::string> json;

    void parseIfComplete();
    void parse();

    void onParsed(std::vector<std::unique_ptr<style::Image>>&&);
    void onError(std::exception_ptr);
};

} // namespace mbgl
