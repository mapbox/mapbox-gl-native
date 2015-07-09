#include <mbgl/text/glyph_store.hpp>

#include <mbgl/text/font_stack.hpp>
#include <mbgl/text/glyph_pbf.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/thread_context.hpp>

namespace mbgl {

GlyphStore::GlyphStore() {
}

GlyphStore::~GlyphStore() {
}

void GlyphStore::requestGlyphRange(const std::string& fontStackName, const GlyphRange& range) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    auto successCallback = [this, fontStackName](GlyphPBF* glyph) {
        try {
            {
                auto fontStack = createFontStack(fontStackName);
                glyph->parse(**fontStack);
            }
            emitGlyphRangeLoaded();
        } catch (const std::exception&) {
            std::string message = "Failed to parse [" + glyph->getURL() + "]";
            emitGlyphRangeLoadingFailed(message);
        }
    };

    auto failureCallback = [this](const std::string& message) {
        emitGlyphRangeLoadingFailed(message);
    };

    std::lock_guard<std::mutex> lock(rangesMutex);
    auto& rangeSets = ranges[fontStackName];

    const auto& rangeSetsIt = rangeSets.find(range);
    if (rangeSetsIt != rangeSets.end()) {
        return;
    }

    rangeSets.emplace(range, std::make_unique<GlyphPBF>(glyphURL, fontStackName, range,
        successCallback, failureCallback));
}


bool GlyphStore::hasGlyphRanges(const std::string& fontStackName, const std::set<GlyphRange>& glyphRanges) {
    if (glyphRanges.empty()) {
        return true;
    }

    std::lock_guard<std::mutex> lock(rangesMutex);
    const auto& rangeSets = ranges[fontStackName];

    bool hasRanges = true;

    for (const auto& range : glyphRanges) {
        const auto& rangeSetsIt = rangeSets.find(range);
        if (rangeSetsIt == rangeSets.end()) {
            // Post the request to the Map thread.
            workQueue.push(std::bind(&GlyphStore::requestGlyphRange, this, fontStackName, range));
            hasRanges = false;
            continue;
        }

        if (!rangeSetsIt->second->isParsed()) {
            hasRanges = false;
        }
    }

    return hasRanges;
}

util::exclusive<FontStack> GlyphStore::createFontStack(const std::string& fontStack) {
    auto lock = std::make_unique<std::lock_guard<std::mutex>>(stacksMutex);

    auto it = stacks.find(fontStack);
    if (it == stacks.end()) {
        it = stacks.emplace(fontStack, std::make_unique<FontStack>()).first;
    }

    return { it->second.get(), std::move(lock) };
}

util::exclusive<FontStack> GlyphStore::getFontStack(const std::string &fontStack) {
    auto lock = std::make_unique<std::lock_guard<std::mutex>>(stacksMutex);

    const auto& it = stacks.find(fontStack);
    if (it == stacks.end()) {
        return { nullptr, nullptr };
    }

    return { it->second.get(), std::move(lock) };
}

void GlyphStore::setObserver(Observer* observer_) {
    observer = observer_;
}

void GlyphStore::emitGlyphRangeLoaded() {
    if (observer) {
        observer->onGlyphRangeLoaded();
    }
}

void GlyphStore::emitGlyphRangeLoadingFailed(const std::string& message) {
    if (!observer) {
        return;
    }

    auto error = std::make_exception_ptr(util::GlyphRangeLoadingException(message));
    observer->onGlyphRangeLoadingFailed(error);
}

}
