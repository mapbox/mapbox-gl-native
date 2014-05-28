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
    // A signed distance field of the glyph with a border of 3 pixels.
    std::string bitmap;

    // Glyph metrics
    GlyphMetrics metrics;
};

class FontStack {
public:
    void insert(uint32_t id, const GlyphMetrics &glyphMetrics, const std::string &bitmap);

    const std::map<uint32_t, GlyphMetrics> &getMetrics() const;

private:
    std::map<uint32_t, std::string> bitmaps;
    std::map<uint32_t, GlyphMetrics> metrics;
    mutable std::mutex mtx;
};

class GlyphPBF {
public:
    GlyphPBF(const std::string &fontStack, GlyphRange glyphRange);

    void parse(FontStack &stack);

    std::shared_future<GlyphPBF &> getFuture();

private:
    std::string data;
    std::promise<GlyphPBF &> promise;
    std::shared_future<GlyphPBF &> future;
    std::mutex mtx;
};

// Manages Glyphrange PBF loading.
class GlyphStore {
public:
    // Block until all specified GlyphRanges of the specified font stack are loaded.
    void waitForGlyphRanges(const std::string &fontStack, const std::set<GlyphRange> &glyphRanges);

    FontStack &getFontStack(const std::string &fontStack);

private:
    // Loads an individual glyph range from the font stack and adds it to rangeSets
    std::shared_future<GlyphPBF &> loadGlyphRange(const std::string &fontStack, std::map<GlyphRange, std::unique_ptr<GlyphPBF>> &rangeSets, GlyphRange range);

    FontStack &createFontStack(const std::string &fontStack);

private:
    std::unordered_map<std::string, std::map<GlyphRange, std::unique_ptr<GlyphPBF>>> ranges;
    std::unordered_map<std::string, std::unique_ptr<FontStack>> stacks;
    std::mutex mtx;
};


}

#endif
