#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/text/glyph_atlas_observer.hpp>
#include <mbgl/text/glyph_pbf.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>

#include <cassert>
#include <algorithm>

namespace mbgl {

static GlyphAtlasObserver nullObserver;

GlyphAtlas::GlyphAtlas(const Size size, FileSource& fileSource_)
    : fileSource(fileSource_),
      observer(&nullObserver),
      bin(size.width, size.height),
      image(size),
      dirty(true) {
}

GlyphAtlas::~GlyphAtlas() = default;

std::map<uint32_t, SDFGlyph>& GlyphAtlas::getGlyphSet(const FontStack& fontStack) {
    return entries[fontStack].sdfs;
}

bool GlyphAtlas::hasGlyphRanges(const FontStack& fontStack, const GlyphRangeSet& ranges) const {
    for (const auto& range : ranges) {
        if (!hasGlyphRange(fontStack,range)) {
            return false;
        }
    }
    return true;
}

bool GlyphAtlas::rangeIsParsed(const std::map<GlyphRange, GlyphRequest>& ranges, const GlyphRange& range) const {
    auto it = ranges.find(range);
    return it != ranges.end() && it->second.parsed;
}

bool GlyphAtlas::hasGlyphRange(const FontStack& fontStack, const GlyphRange& range) const {
    auto it = entries.find(fontStack);
    return it != entries.end() && rangeIsParsed(it->second.ranges, range);
}
    
void GlyphAtlas::getGlyphs(GlyphRequestor& requestor, GlyphDependencies glyphDependencies) {
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
            ranges.insert(getGlyphRange(glyphID));
        }

        for (const auto& range : ranges) {
            if (!rangeIsParsed(entry.ranges, range)) {
                GlyphRequest& request = requestRange(entry, fontStack, range);
                request.requestors[&requestor] = dependencies;
            }
        }
    }

    // If the shared dependencies pointer is already unique, then all dependent glyph ranges
    // have already been loaded. Send a notification immediately.
    if (dependencies.unique()) {
        addGlyphs(requestor, *dependencies);
    }
}

GlyphAtlas::GlyphRequest& GlyphAtlas::requestRange(Entry& entry, const FontStack& fontStack, const GlyphRange& range) {
    GlyphRequest& request = entry.ranges[range];

    if (request.req) {
        return request;
    }

    request.req = fileSource.request(Resource::glyphs(glyphURL, fontStack, range), [this, fontStack, range](Response res) {
        processResponse(res, fontStack, range);
    });

    return request;
}

void GlyphAtlas::processResponse(const Response& res, const FontStack& fontStack, const GlyphRange& range) {
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
        std::vector<SDFGlyph> glyphs;

        try {
            glyphs = parseGlyphPBF(range, *res.data);
        } catch (...) {
            observer->onGlyphsError(fontStack, range, std::current_exception());
            return;
        }

        for (auto& glyph : glyphs) {
            auto it = entry.sdfs.find(glyph.id);
            if (it == entry.sdfs.end()) {
                // Glyph doesn't exist yet.
                entry.sdfs.emplace(glyph.id, std::move(glyph));
            } else if (it->second.metrics == glyph.metrics) {
                if (it->second.bitmap != glyph.bitmap) {
                    // The actual bitmap was updated; this is unsupported.
                    Log::Warning(Event::Glyph, "Modified glyph changed bitmap represenation");
                }
                // At least try to update it in case it's currently unused.
                // If it is already used, we won't attempt to update the glyph atlas texture.
                it->second.bitmap = std::move(glyph.bitmap);
            } else {
                // The metrics were updated; this is unsupported.
                Log::Warning(Event::Glyph, "Modified glyph has different metrics");
                return;
            }
        }
    }

    request.parsed = true;

    for (auto& pair : request.requestors) {
        GlyphRequestor& requestor = *pair.first;
        const std::shared_ptr<GlyphDependencies>& dependencies = pair.second;
        if (dependencies.unique()) {
            addGlyphs(requestor, *dependencies);
        }
    }

    request.requestors.clear();

    observer->onGlyphsLoaded(fontStack, range);
}

void GlyphAtlas::setObserver(GlyphAtlasObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver;
}

void GlyphAtlas::addGlyphs(GlyphRequestor& requestor, const GlyphDependencies& glyphDependencies) {
    GlyphPositionMap glyphPositions;
    GlyphRangeSet loadedRanges;

    for (const auto& dependency : glyphDependencies) {
        const FontStack& fontStack = dependency.first;
        const GlyphIDs& glyphIDs = dependency.second;

        GlyphPositions& positions = glyphPositions[fontStack];
        Entry& entry = entries[fontStack];

        for (const auto& glyphID : glyphIDs) {
            loadedRanges.insert(getGlyphRange(glyphID));
            auto it = entry.sdfs.find(glyphID);
            if (it == entry.sdfs.end())
                continue;

            addGlyph(requestor, fontStack, it->second);

            // It's possible to have an SDF without a valid position (if the SDF was malformed).
            // We indicate this case with Rect<uint16_t>(0,0,0,0).
            auto glyphRect = entry.glyphValues.find(glyphID);
            const Rect<uint16_t> rect = glyphRect == entry.glyphValues.end()
                ? Rect<uint16_t>(0,0,0,0)
                : glyphRect->second.rect;

            positions.emplace(std::piecewise_construct,
                              std::forward_as_tuple(glyphID),
                              std::forward_as_tuple(rect, it->second.metrics));
        }
    }

    requestor.onGlyphsAvailable(glyphPositions, loadedRanges);
}

void GlyphAtlas::addGlyph(GlyphRequestor& requestor,
                          const FontStack& fontStack,
                          const SDFGlyph& glyph)
{
    std::map<uint32_t, GlyphValue>& face = entries[fontStack].glyphValues;
    auto it = face.find(glyph.id);

    // The glyph is already in this texture.
    if (it != face.end()) {
        GlyphValue& value = it->second;
        value.ids.insert(&requestor);
        return;
    }

    // Guard against glyphs that are too large, or that we don't need to place into the atlas since
    // they don't have any pixels.
    if (glyph.metrics.width == 0 || glyph.metrics.width >= 256 ||
        glyph.metrics.height == 0 || glyph.metrics.height >= 256) {
        return;
    }

    // Add a 1px border around every image.
    const uint32_t padding = 1;
    uint16_t width = glyph.bitmap.size.width + 2 * padding;
    uint16_t height = glyph.bitmap.size.height + 2 * padding;

    // Increase to next number divisible by 4, but at least 1.
    // This is so we can scale down the texture coordinates and pack them
    // into 2 bytes rather than 4 bytes.
    width += (4 - width % 4);
    height += (4 - height % 4);

    Rect<uint16_t> rect = bin.allocate(width, height);
    if (rect.w == 0) {
        Log::Error(Event::OpenGL, "glyph bitmap overflow");
        return;
    }

    face.emplace(glyph.id, GlyphValue { rect, &requestor });

    AlphaImage::copy(glyph.bitmap, image, { 0, 0 }, { rect.x + padding, rect.y + padding }, glyph.bitmap.size);

    dirty = true;
}

void GlyphAtlas::removeGlyphValues(GlyphRequestor& requestor, std::map<uint32_t, GlyphValue>& face) {
    for (auto it = face.begin(); it != face.end(); /* we advance in the body */) {
        GlyphValue& value = it->second;
        value.ids.erase(&requestor);

        if (value.ids.empty()) {
            const Rect<uint16_t>& rect = value.rect;

            // Clear out the bitmap.
            uint8_t *target = image.data.get();
            for (uint32_t y = 0; y < rect.h; y++) {
                uint32_t y1 = image.size.width * (rect.y + y) + rect.x;
                for (uint32_t x = 0; x < rect.w; x++) {
                    target[y1 + x] = 0;
                }
            }

            bin.release(rect);

            // Make sure to post-increment the iterator: This will return the
            // current iterator, but will go to the next position before we
            // erase the element from the map. That way, the iterator stays
            // valid.
            face.erase(it++);
        } else {
            ++it;
        }
    }
}

void GlyphAtlas::removePendingRanges(mbgl::GlyphRequestor& requestor, std::map<GlyphRange, GlyphRequest>& ranges) {
    for (auto it = ranges.begin(); it != ranges.end(); it++) {
        it->second.requestors.erase(&requestor);
    }
}

void GlyphAtlas::removeGlyphs(GlyphRequestor& requestor) {
    for (auto& entry : entries) {
        removeGlyphValues(requestor, entry.second.glyphValues);
        removePendingRanges(requestor, entry.second.ranges);
    }
}

Size GlyphAtlas::getSize() const {
    return image.size;
}

void GlyphAtlas::upload(gl::Context& context, gl::TextureUnit unit) {
    if (!texture) {
        texture = context.createTexture(image, unit);
    } else if (dirty) {
        context.updateTexture(*texture, image, unit);
    }
    
    dirty = false;
}

void GlyphAtlas::bind(gl::Context& context, gl::TextureUnit unit) {
    upload(context, unit);
    context.bindTexture(*texture, unit, gl::TextureFilter::Linear);
}

} // namespace mbgl
