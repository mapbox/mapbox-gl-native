#include <mbgl/text/glyph_store.hpp>

#include <mbgl/text/glyph_pbf.hpp>
#include <mbgl/util/thread_context.hpp>

namespace mbgl {

void GlyphStore::requestGlyphRange(const std::string& fontStackName, const GlyphRange& range) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    std::lock_guard<std::mutex> lock(rangesMutex);
    auto& rangeSets = ranges[fontStackName];

    const auto& rangeSetsIt = rangeSets.find(range);
    if (rangeSetsIt != rangeSets.end()) {
        return;
    }

    auto glyphPBF = std::make_unique<GlyphPBF>(this, fontStackName, range);
    glyphPBF->setObserver(this);

    rangeSets.emplace(range, std::move(glyphPBF));
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
            // Push the request to the MapThread, so we can easly cancel
            // if it is still pending when we destroy this object.
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

util::exclusive<FontStack> GlyphStore::getFontStack(const std::string& fontStack) {
    auto lock = std::make_unique<std::lock_guard<std::mutex>>(stacksMutex);

    auto it = stacks.find(fontStack);
    if (it == stacks.end()) {
        it = stacks.emplace(fontStack, std::make_unique<FontStack>()).first;
    }

    // FIXME: We lock all FontStacks, but what we should
    // really do is lock only the one we are returning.
    return { it->second.get(), std::move(lock) };
}

void GlyphStore::onGlyphPBFLoaded() {
    if (observer) {
        observer->onGlyphRangeLoaded();
    }
}

void GlyphStore::onGlyphPBFLoadingFailed(std::exception_ptr error) {
    if (observer) {
        observer->onGlyphRangeLoadingFailed(error);
    }
}

void GlyphStore::setObserver(Observer* observer_) {
    observer = observer_;
}

}
