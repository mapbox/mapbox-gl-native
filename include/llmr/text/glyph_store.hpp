#ifndef LLMR_TEXT_GLYPH_STORE
#define LLMR_TEXT_GLYPH_STORE

#include <llmr/text/glyph.hpp>
#include <llmr/util/pbf.hpp>


#include <cstdint>
#include <vector>
#include <future>
#include <map>
#include <set>
#include <unordered_map>

namespace llmr {


class SDFGlyph {
public:
    SDFGlyph();
    SDFGlyph(pbf data);

    // A signed distance field of the glyph with a border of 3 pixels.
    std::string bitmap;

    // Glyph metrics
    GlyphMetrics metrics;
};


class GlyphSet {
public:
    GlyphSet(const std::string &fontStack, GlyphRange glyphRange);

    void parse();

    std::shared_future<GlyphSet &> getFuture();

private:
    std::map<uint32_t, SDFGlyph> glyphs;

    std::string data;
    std::promise<GlyphSet &> promise;
    std::shared_future<GlyphSet &> future;
    std::mutex mtx;
};

// Manages Glyphrange PBF loading.
class GlyphStore {
public:
    // Block until all specified GlyphRanges of the specified font stack are loaded.
    void waitForGlyphRanges(const std::string &fontStack, const std::set<GlyphRange> &glyphRanges);

private:
    // Loads an individual glyph range from the font stack and adds it to rangeSets
    std::shared_future<GlyphSet &> loadGlyphRange(const std::string &fontStack, std::map<GlyphRange, std::unique_ptr<GlyphSet>> &rangeSets, GlyphRange range);

private:
    std::unordered_map<std::string, std::map<GlyphRange, std::unique_ptr<GlyphSet>>> stacks;
    std::mutex mtx;
};


}

#endif
