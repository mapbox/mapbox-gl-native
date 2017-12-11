#include <mbgl/text/glyph_manager.hpp>
#include <mbgl/text/glyph_manager_observer.hpp>
#include <mbgl/text/glyph_pbf.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/tiny_sdf.hpp>

namespace mbgl {

static GlyphManagerObserver nullObserver;

GlyphManager::GlyphManager(FileSource& fileSource_, std::unique_ptr<LocalGlyphRasterizer> localGlyphRasterizer_)
    : fileSource(fileSource_),
      observer(&nullObserver),
      localGlyphRasterizer(std::move(localGlyphRasterizer_)) {
}

GlyphManager::~GlyphManager() = default;

void GlyphManager::getGlyphs(GlyphRequestor& requestor, GlyphDependencies glyphDependencies) {
    auto dependencies = std::make_shared<GlyphDependencies>(std::move(glyphDependencies));

    // Figure out which glyph ranges need to be fetched. For each range that does need to
    // be fetched, record an entry mapping the requestor to a shared pointer containing the
    // dependencies. When the shared pointer becomes unique, we know that all the dependencies
    // for that requestor have been fetched, and can notify it of completion.
    for (const auto& dependency : *dependencies) {
        const FontStack& fontStack = dependency.first;
        Entry& entry = entries[fontStack];

        const GlyphIDs& glyphIDs = dependency.second;
        GlyphRangeSet ranges;
        for (const auto& glyphID : glyphIDs) {
            if (localGlyphRasterizer->canRasterizeGlyph(fontStack, glyphID)) {
                if (entry.glyphs.find(glyphID) == entry.glyphs.end()) {
                    entry.glyphs.emplace(glyphID, makeMutable<Glyph>(generateLocalSDF(fontStack, glyphID)));
                }
            } else {
                ranges.insert(getGlyphRange(glyphID));
            }
        }

        for (const auto& range : ranges) {
            auto it = entry.ranges.find(range);
            if (it == entry.ranges.end() || !it->second.parsed) {
                GlyphRequest& request = entry.ranges[range];
                request.requestors[&requestor] = dependencies;
                requestRange(request, fontStack, range);
            }
        }
    }

    // If the shared dependencies pointer is already unique, then all dependent glyph ranges
    // have already been loaded. Send a notification immediately.
    if (dependencies.unique()) {
        notify(requestor, *dependencies);
    }
}

Glyph GlyphManager::generateLocalSDF(const FontStack& fontStack, GlyphID glyphID) {
    Glyph local = localGlyphRasterizer->rasterizeGlyph(fontStack, glyphID);
    local.bitmap = util::transformRasterToSDF(local.bitmap, 8, .25);
    return local;
}

void GlyphManager::requestRange(GlyphRequest& request, const FontStack& fontStack, const GlyphRange& range) {
    if (request.req) {
        return;
    }

    request.req = fileSource.request(Resource::glyphs(glyphURL, fontStack, range), [this, fontStack, range](Response res) {
        processResponse(res, fontStack, range);
    });
}

void GlyphManager::processResponse(const Response& res, const FontStack& fontStack, const GlyphRange& range) {
    if (res.error) {
        observer->onGlyphsError(fontStack, range, std::make_exception_ptr(std::runtime_error(res.error->message)));
        return;
    }

    if (res.notModified) {
        return;
    }

    Entry& entry = entries[fontStack];
    GlyphRequest& request = entry.ranges[range];

    if (!res.noContent) {
        std::vector<Glyph> glyphs;

        try {
            glyphs = parseGlyphPBF(range, *res.data);
        } catch (...) {
            observer->onGlyphsError(fontStack, range, std::current_exception());
            return;
        }

        for (auto& glyph : glyphs) {
            entry.glyphs.erase(glyph.id);
            entry.glyphs.emplace(glyph.id, makeMutable<Glyph>(std::move(glyph)));
        }
    }

    request.parsed = true;

    for (auto& pair : request.requestors) {
        GlyphRequestor& requestor = *pair.first;
        const std::shared_ptr<GlyphDependencies>& dependencies = pair.second;
        if (dependencies.unique()) {
            notify(requestor, *dependencies);
        }
    }

    request.requestors.clear();

    observer->onGlyphsLoaded(fontStack, range);
}

void GlyphManager::setObserver(GlyphManagerObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver;
}

void GlyphManager::notify(GlyphRequestor& requestor, const GlyphDependencies& glyphDependencies) {
    GlyphMap response;

    for (const auto& dependency : glyphDependencies) {
        const FontStack& fontStack = dependency.first;
        const GlyphIDs& glyphIDs = dependency.second;

        Glyphs& glyphs = response[fontStack];
        Entry& entry = entries[fontStack];

        for (const auto& glyphID : glyphIDs) {
            auto it = entry.glyphs.find(glyphID);
            if (it != entry.glyphs.end()) {
                glyphs.emplace(*it);
            } else {
                glyphs.emplace(glyphID, std::experimental::nullopt);
            }
        }
    }

    requestor.onGlyphsAvailable(response);
}

void GlyphManager::removeRequestor(GlyphRequestor& requestor) {
    for (auto& entry : entries) {
        for (auto& range : entry.second.ranges) {
            range.second.requestors.erase(&requestor);
        }
    }
}

} // namespace mbgl
