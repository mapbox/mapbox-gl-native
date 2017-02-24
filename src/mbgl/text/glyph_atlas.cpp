#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/text/glyph_atlas_observer.hpp>
#include <mbgl/text/glyph_pbf.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>

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

void GlyphAtlas::requestGlyphRange(const FontStack& fontStack, const GlyphRange& range) {
    std::lock_guard<std::mutex> lock(mutex);
    auto& rangeSets = entries[fontStack].ranges;

    const auto& rangeSetsIt = rangeSets.find(range);
    if (rangeSetsIt != rangeSets.end()) {
        return;
    }

    rangeSets.emplace(std::piecewise_construct,
                      std::forward_as_tuple(range),
                      std::forward_as_tuple(this, fontStack, range, observer, fileSource));
}

bool GlyphAtlas::hasGlyphRanges(const FontStack& fontStack, const GlyphRangeSet& glyphRanges) {
    if (glyphRanges.empty()) {
        return true;
    }

    std::lock_guard<std::mutex> lock(mutex);
    const auto& rangeSets = entries[fontStack].ranges;

    bool hasRanges = true;
    for (const auto& range : glyphRanges) {
        const auto& rangeSetsIt = rangeSets.find(range);
        if (rangeSetsIt == rangeSets.end()) {
            // Push the request to the MapThread, so we can easly cancel
            // if it is still pending when we destroy this object.
            workQueue.push(std::bind(&GlyphAtlas::requestGlyphRange, this, fontStack, range));

            hasRanges = false;
            continue;
        }

        if (!rangeSetsIt->second.isParsed()) {
            hasRanges = false;
        }
    }

    return hasRanges;
}

util::exclusive<GlyphSet> GlyphAtlas::getGlyphSet(const FontStack& fontStack) {
    auto lock = std::make_unique<std::lock_guard<std::mutex>>(mutex);
    return { &entries[fontStack].glyphSet, std::move(lock) };
}

void GlyphAtlas::setObserver(GlyphAtlasObserver* observer_) {
    observer = observer_;
}

void GlyphAtlas::addGlyphs(uintptr_t tileUID,
                           const std::u16string& text,
                           const FontStack& fontStack,
                           const util::exclusive<GlyphSet>& glyphSet,
                           GlyphPositions& face)
{
    const std::map<uint32_t, SDFGlyph>& sdfs = glyphSet->getSDFs();

    for (char16_t chr : text)
    {
        auto sdf_it = sdfs.find(chr);
        if (sdf_it == sdfs.end()) {
            continue;
        }

        const SDFGlyph& sdf = sdf_it->second;
        Rect<uint16_t> rect = addGlyph(tileUID, fontStack, sdf);
        face.emplace(chr, Glyph{rect, sdf.metrics});
    }
}

Rect<uint16_t> GlyphAtlas::addGlyph(uintptr_t tileUID,
                                    const FontStack& fontStack,
                                    const SDFGlyph& glyph)
{
    std::map<uint32_t, GlyphValue>& face = entries[fontStack].glyphValues;
    auto it = face.find(glyph.id);

    // The glyph is already in this texture.
    if (it != face.end()) {
        GlyphValue& value = it->second;
        value.ids.insert(tileUID);
        return value.rect;
    }

    // Guard against glyphs that are too large, or that we don't need to place into the atlas since
    // they don't have any pixels.
    if (glyph.metrics.width == 0 || glyph.metrics.width >= 256 ||
        glyph.metrics.height == 0 || glyph.metrics.height >= 256) {
        return Rect<uint16_t>{ 0, 0, 0, 0 };
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
        return rect;
    }

    face.emplace(glyph.id, GlyphValue { rect, tileUID });

    AlphaImage::copy(glyph.bitmap, image, { 0, 0 }, { rect.x + padding, rect.y + padding }, glyph.bitmap.size);

    dirty = true;

    return rect;
}

void GlyphAtlas::removeGlyphs(uintptr_t tileUID) {
    std::lock_guard<std::mutex> lock(mutex);

    for (auto& entry : entries) {
        std::map<uint32_t, GlyphValue>& face = entry.second.glyphValues;
        for (auto it = face.begin(); it != face.end(); /* we advance in the body */) {
            GlyphValue& value = it->second;
            value.ids.erase(tileUID);

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
