#include <llmr/text/glyph_store.hpp>

#include <llmr/util/std.hpp>
#include <llmr/util/string.hpp>
#include <llmr/util/utf.hpp>
#include <llmr/util/pbf.hpp>
#include <llmr/util/constants.hpp>
#include <llmr/platform/platform.hpp>
#include <uv.h>
#include <algorithm>

namespace llmr {


void FontStack::insert(uint32_t id, const SDFGlyph &glyph) {
    std::lock_guard<std::mutex> lock(mtx);
    metrics.emplace(id, glyph.metrics);
    bitmaps.emplace(id, glyph.bitmap);
    sdfs.emplace(id, glyph);
}

const std::map<uint32_t, GlyphMetrics> &FontStack::getMetrics() const {
    std::lock_guard<std::mutex> lock(mtx);
    return metrics;
}

const std::map<uint32_t, SDFGlyph> &FontStack::getSDFs() const {
    std::lock_guard<std::mutex> lock(mtx);
    return sdfs;
}

const Shaping FontStack::getShaping(const std::u32string &string, const float &maxWidth,
        const float &lineHeight, const float &alignment, const float &verticalAlignment,
        const float &letterSpacing) const {

    std::lock_guard<std::mutex> lock(mtx);
    uint32_t i = 0;
    uint32_t x = 0;
    Shaping shaping;
    // Loop through all characters of this label and shape.
    for (uint32_t chr : string) {
        shaping.emplace_back(0, chr, x, 0);
        i++;
        auto metric = metrics.find(chr);
        if (metric != metrics.end()) {
            x += metric->second.advance + letterSpacing;
        }
    }

    lineWrap(shaping, lineHeight, maxWidth, alignment, verticalAlignment);

    return shaping;
}

void alignVertically(Shaping &shaping, const uint32_t &lines, const float &lineHeight, const float &verticalAlign) {
    const float dy = -(lineHeight * (lines - 1) + 24) * verticalAlign - 5;
    for (GlyphPlacement &shape : shaping) {
        shape.y += dy;
    }
}

void alignHorizontally(Shaping &shaping, const std::map<uint32_t, GlyphMetrics> &metrics,
        const uint32_t &start, const uint32_t &end, const float &alignment) {
    auto & shape = shaping.at(end);
    auto metric = metrics.find(shape.glyph);
    if (metric != metrics.end()) {
        uint32_t lastAdvance = metric->second.advance;
        int32_t lineIndent = (shape.x + lastAdvance) * alignment;
        for (uint32_t j = start; j <= end; j++) {
            shaping[j].x -= lineIndent;
        }
    }
}

void FontStack::lineWrap(Shaping &shaping, const float &lineHeight, const float &maxWidth,
        const float &alignment, const float &verticalAlignment) const {

    std::size_t num_shapes = shaping.size();
    if (!num_shapes) {
        return;
    }
    uint32_t lastSafeBreak = 0;
    uint32_t lengthBeforeCurrentLine = 0;
    uint32_t lineStartIndex = 0;
    uint32_t line = 0;

    for (uint32_t i = 0; i < shaping.size(); i++) {
        GlyphPlacement &shape = shaping[i];
        shape.x -= lengthBeforeCurrentLine;
        shape.y += lineHeight * line;

        if (shape.x > maxWidth && lastSafeBreak > 0) {

            uint32_t lineLength = shaping[lastSafeBreak + 1].x;

            for (uint32_t k = lastSafeBreak + 1; k <= i; k++) {
                shaping[k].y += lineHeight;
                shaping[k].x -= lineLength;
            }

            if (alignment) {
                alignHorizontally(shaping, metrics, lineStartIndex, lastSafeBreak - 1, alignment);
            }

            lineStartIndex = lastSafeBreak + 1;
            lastSafeBreak = 0;
            lengthBeforeCurrentLine += lineLength;
            line++;
        }
        if (shape.glyph == 32) {
            lastSafeBreak = i;
        }
    }
    alignHorizontally(shaping, metrics, lineStartIndex, shaping.size() - 1, alignment);
    alignVertically(shaping, line + 1, lineHeight, verticalAlignment);
}

GlyphPBF::GlyphPBF(const std::string &fontStack, GlyphRange glyphRange)
    : future(promise.get_future().share())
{
    // Load the glyph set URL
    std::string url = util::sprintf<255>(kGlyphURL, fontStack.c_str(), glyphRange.first, glyphRange.second);

    // TODO: Find more reliable URL normalization function
    std::replace(url.begin(), url.end(), ' ', '+');

#if defined(DEBUG)
    fprintf(stderr, "%s\n", url.c_str());
#endif

    platform::request_http(url, [&](platform::Response *res) {
        if (res->code != 200) {
            // Something went wrong with loading the glyph pbf. Pass on the error to the future listeners.
            const std::string msg = util::sprintf<255>("[ERROR] failed to load glyphs (%d): %s\n", res->code, res->error_message.c_str());
            promise.set_exception(std::make_exception_ptr(std::runtime_error(msg)));
        } else {
            // Transfer the data to the GlyphSet and signal its availability.
            // Once it is available, the caller will need to call parse() to actually
            // parse the data we received. We are not doing this here since this callback is being
            // called from another (unknown) thread.
            data.swap(res->body);
            promise.set_value(*this);
        }
    });
}

std::shared_future<GlyphPBF &> GlyphPBF::getFuture() {
    return future;
}

void GlyphPBF::parse(FontStack &stack) {
    std::lock_guard<std::mutex> lock(mtx);

    if (!data.size()) {
        // If there is no data, this means we either haven't received any data, or
        // we have already parsed the data.
        return;
    }

    // Parse the glyph PBF
    pbf glyphs_pbf(reinterpret_cast<const uint8_t *>(data.data()), data.size());

    while (glyphs_pbf.next()) {
        if (glyphs_pbf.tag == 1) { // stacks
            pbf fontstack_pbf = glyphs_pbf.message();
            while (fontstack_pbf.next()) {
                if (fontstack_pbf.tag == 3) { // glyphs
                    pbf glyph_pbf = fontstack_pbf.message();

                    SDFGlyph glyph;

                    while (glyph_pbf.next()) {
                        if (glyph_pbf.tag == 1) { // id
                            glyph.id = glyph_pbf.varint();
                        } else if (glyph_pbf.tag == 2) { // bitmap
                            glyph.bitmap = glyph_pbf.string();
                        } else if (glyph_pbf.tag == 3) { // width
                            glyph.metrics.width = glyph_pbf.varint();
                        } else if (glyph_pbf.tag == 4) { // height
                            glyph.metrics.height = glyph_pbf.varint();
                        } else if (glyph_pbf.tag == 5) { // left
                            glyph.metrics.left = glyph_pbf.svarint();
                        } else if (glyph_pbf.tag == 6) { // top
                            glyph.metrics.top = glyph_pbf.svarint();
                        } else if (glyph_pbf.tag == 7) { // advance
                            glyph.metrics.advance = glyph_pbf.varint();
                        } else {
                            glyph_pbf.skip();
                        }
                    }

                    stack.insert(glyph.id, glyph);
                } else {
                    fontstack_pbf.skip();
                }
            }
        } else {
            glyphs_pbf.skip();
        }
    }

    data.clear();
}


void GlyphStore::waitForGlyphRanges(const std::string &fontStack, const std::set<GlyphRange> &glyphRanges) {
    // We are implementing a blocking wait with futures: Every GlyphSet has a future that we are
    // waiting for until it is loaded.

    FontStack *stack = nullptr;

    std::vector<std::shared_future<GlyphPBF &>> futures;
    futures.reserve(glyphRanges.size());
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto &rangeSets = ranges[fontStack];

        stack = &createFontStack(fontStack);

        // Attempt to load the glyph range. If the GlyphSet already exists, we are getting back
        // the same shared_future.
        for (GlyphRange range : glyphRanges) {
            futures.emplace_back(loadGlyphRange(fontStack, rangeSets, range));
        }
    }

    // Now that we potentially created all GlyphSets, we are waiting for the results, one by one.
    // When we get a result (or the GlyphSet is aready loaded), we are attempting to parse the
    // GlyphSet.
    for (std::shared_future<GlyphPBF &> &future : futures) {
        future.get().parse(*stack);
    }
}

std::shared_future<GlyphPBF &> GlyphStore::loadGlyphRange(const std::string &name, std::map<GlyphRange, std::unique_ptr<GlyphPBF>> &rangeSets, const GlyphRange range) {
    auto range_it = rangeSets.find(range);
    if (range_it == rangeSets.end()) {
        // We don't have this glyph set yet for this font stack.
        range_it = rangeSets.emplace(range, std::make_unique<GlyphPBF>(name, range)).first;
    }

    return range_it->second->getFuture();
}

FontStack &GlyphStore::createFontStack(const std::string &fontStack) {
    auto stack_it = stacks.find(fontStack);
    if (stack_it == stacks.end()) {
        stack_it = stacks.emplace(fontStack, std::make_unique<FontStack>()).first;
    }
    return *stack_it->second.get();
}

FontStack &GlyphStore::getFontStack(const std::string &fontStack) {
    std::lock_guard<std::mutex> lock(mtx);
    return createFontStack(fontStack);
}


}
