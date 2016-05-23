#include <mbgl/text/glyph_store.hpp>
#include <mbgl/text/glyph_store_observer.hpp>
#include <mbgl/text/glyph_pbf.hpp>

#include <cassert>

namespace mbgl {

static GlyphStoreObserver nullObserver;

GlyphStore::GlyphStore(FileSource& fileSource_)
    : fileSource(fileSource_), observer(&nullObserver) {
}

GlyphStore::~GlyphStore() = default;

void GlyphStore::requestGlyphRange(const FontStack& fontStack, const GlyphRange& range) {
    std::lock_guard<std::mutex> lock(rangesMutex);
    auto& rangeSets = ranges[fontStack];

    const auto& rangeSetsIt = rangeSets.find(range);
    if (rangeSetsIt != rangeSets.end()) {
        return;
    }

    rangeSets.emplace(range,
        std::make_unique<GlyphPBF>(this, fontStack, range, observer, fileSource));
}


bool GlyphStore::hasGlyphRanges(const FontStack& fontStack, const std::set<GlyphRange>& glyphRanges) {
    if (glyphRanges.empty()) {
        return true;
    }

    std::lock_guard<std::mutex> lock(rangesMutex);
    const auto& rangeSets = ranges[fontStack];

    bool hasRanges = true;
    for (const auto& range : glyphRanges) {
        const auto& rangeSetsIt = rangeSets.find(range);
        if (rangeSetsIt == rangeSets.end()) {
            // Push the request to the MapThread, so we can easly cancel
            // if it is still pending when we destroy this object.
            workQueue.push(std::bind(&GlyphStore::requestGlyphRange, this, fontStack, range));

            hasRanges = false;
            continue;
        }

        if (!rangeSetsIt->second->isParsed()) {
            hasRanges = false;
        }
    }

    return hasRanges;
}

util::exclusive<GlyphSet> GlyphStore::getGlyphSet(const FontStack& fontStack) {
    auto lock = std::make_unique<std::lock_guard<std::mutex>>(glyphSetsMutex);

    auto it = glyphSets.find(fontStack);
    if (it == glyphSets.end()) {
        it = glyphSets.emplace(fontStack, std::make_unique<GlyphSet>()).first;
    }

    // FIXME: We lock all GlyphSets, but what we should
    // really do is lock only the one we are returning.
    return { it->second.get(), std::move(lock) };
}

void GlyphStore::setObserver(GlyphStoreObserver* observer_) {
    observer = observer_;
}

} // namespace mbgl
