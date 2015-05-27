#include <mbgl/text/glyph_store.hpp>
#include <mbgl/text/glyph_pbf.hpp>
#include <mbgl/text/font_stack.hpp>

#include <mbgl/util/exception.hpp>
#include <mbgl/util/uv_detail.hpp>

namespace mbgl {

GlyphStore::GlyphStore(uv_loop_t* loop, Environment& env_)
    : env(env_),
      asyncEmitGlyphRangeLoaded(std::make_unique<uv::async>(loop, [this] { emitGlyphRangeLoaded(); })),
      asyncEmitGlyphRangeLoadedingFailed(std::make_unique<uv::async>(loop, [this] { emitGlyphRangeLoadingFailed(); })),
      observer(nullptr) {
    asyncEmitGlyphRangeLoaded->unref();
    asyncEmitGlyphRangeLoadedingFailed->unref();
}

GlyphStore::~GlyphStore() {
    observer = nullptr;
}

void GlyphStore::setURL(const std::string &url) {
    glyphURL = url;
}

bool GlyphStore::requestGlyphRangesIfNeeded(const std::string& fontStackName,
                                            const std::set<GlyphRange>& glyphRanges) {
    bool requestIsNeeded = false;

    if (glyphRanges.empty()) {
        return requestIsNeeded;
    }

    auto successCallback = [this, fontStackName](GlyphPBF* glyph) {
        auto fontStack = createFontStack(fontStackName);
        try {
            glyph->parse(**fontStack);
            asyncEmitGlyphRangeLoaded->send();
        } catch (const std::exception&) {
            std::lock_guard<std::mutex> lock(errorMessageMutex);
            errorMessage = "Failed to parse [" + glyph->getURL() + "]";
            asyncEmitGlyphRangeLoadedingFailed->send();
        }
    };

    auto failureCallback = [this](const std::string& message) {
        std::lock_guard<std::mutex> lock(errorMessageMutex);
        errorMessage = message;
        asyncEmitGlyphRangeLoadedingFailed->send();
    };

    std::lock_guard<std::mutex> lock(rangesMutex);
    auto& rangeSets = ranges[fontStackName];

    for (const auto& range : glyphRanges) {
        const auto& rangeSets_it = rangeSets.find(range);
        if (rangeSets_it == rangeSets.end()) {
            auto glyph = std::make_unique<GlyphPBF>(glyphURL, fontStackName, range, env,
                successCallback, failureCallback);
            rangeSets.emplace(range, std::move(glyph));
            requestIsNeeded = true;
            continue;
        }

        if (!rangeSets_it->second->isParsed()) {
            requestIsNeeded = true;
        }
    }

    return requestIsNeeded;
}

util::exclusive<FontStack> GlyphStore::createFontStack(const std::string &fontStack) {
    auto lock = std::make_unique<std::lock_guard<std::mutex>>(stacksMutex);

    auto stack_it = stacks.find(fontStack);
    if (stack_it == stacks.end()) {
        stack_it = stacks.emplace(fontStack, std::make_unique<FontStack>()).first;
    }

    return { stack_it->second.get(), std::move(lock) };
}

util::exclusive<FontStack> GlyphStore::getFontStack(const std::string &fontStack) {
    auto lock = std::make_unique<std::lock_guard<std::mutex>>(stacksMutex);

    const auto& stack_it = stacks.find(fontStack);
    if (stack_it == stacks.end()) {
        return { nullptr, nullptr };
    }

    return { stack_it->second.get(), std::move(lock) };
}

void GlyphStore::setObserver(Observer* observer_) {
    observer = observer_;
}

void GlyphStore::emitGlyphRangeLoaded() {
    if (observer) {
        observer->onGlyphRangeLoaded();
    }
}

void GlyphStore::emitGlyphRangeLoadingFailed() {
    if (!observer) {
        return;
    }

    std::lock_guard<std::mutex> lock(errorMessageMutex);
    auto error = std::make_exception_ptr(util::GlyphRangeLoadingException(errorMessage));
    observer->onGlyphRangeLoadingFailed(error);
}

}
