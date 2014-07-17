#ifndef MBGL_TEXT_GLYPH_STORE
#define MBGL_TEXT_GLYPH_STORE

#include <mbgl/text/glyph.hpp>
#include <mbgl/util/pbf.hpp>

#include <cstdint>
#include <vector>
#include <future>
#include <map>
#include <set>
#include <unordered_map>

namespace mbgl {


class SDFGlyph {
public:
    uint32_t id = 0;

    // A signed distance field of the glyph with a border of 3 pixels.
    std::string bitmap;

    // Glyph metrics
    GlyphMetrics metrics;
};

class FontStack {
public:
    void insert(uint32_t id, const SDFGlyph &glyph);
    const std::map<uint32_t, GlyphMetrics> &getMetrics() const;
    const std::map<uint32_t, SDFGlyph> &getSDFs() const;
    const Shaping getShaping(const std::u32string &string,
                             const float &maxWidth,
                             const float &lineHeight,
                             const float &alignment,
                             const float &verticalAlignment,
                             const float &letterSpacing) const;
    void lineWrap(Shaping &shaping,
                     const float &lineHeight,
                     const float &maxWidth,
                     const float &alignment,
                     const float &verticalAlignment) const;

private:
    std::map<uint32_t, std::string> bitmaps;
    std::map<uint32_t, GlyphMetrics> metrics;
    std::map<uint32_t, SDFGlyph> sdfs;
    mutable std::mutex mtx;
};

class GlyphPBF {
public:
    GlyphPBF(const std::string &glyphURL, const std::string &fontStack, GlyphRange glyphRange);

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
    GlyphStore(const std::string &glyphURL);

    // Block until all specified GlyphRanges of the specified font stack are loaded.
    void waitForGlyphRanges(const std::string &fontStack, const std::set<GlyphRange> &glyphRanges);

    FontStack &getFontStack(const std::string &fontStack);

private:
    // Loads an individual glyph range from the font stack and adds it to rangeSets
    std::shared_future<GlyphPBF &> loadGlyphRange(const std::string &fontStack, std::map<GlyphRange, std::unique_ptr<GlyphPBF>> &rangeSets, GlyphRange range);

    FontStack &createFontStack(const std::string &fontStack);

public:
    const std::string glyphURL;

private:
    std::unordered_map<std::string, std::map<GlyphRange, std::unique_ptr<GlyphPBF>>> ranges;
    std::unordered_map<std::string, std::unique_ptr<FontStack>> stacks;
    std::mutex mtx;
};


}

#endif
