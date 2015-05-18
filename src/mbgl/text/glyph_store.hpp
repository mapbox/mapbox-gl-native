#ifndef MBGL_TEXT_GLYPH_STORE
#define MBGL_TEXT_GLYPH_STORE

#include <mbgl/text/glyph.hpp>
#include <mbgl/util/vec.hpp>
#include <mbgl/util/ptr.hpp>

#include <cstdint>
#include <vector>
#include <future>
#include <map>
#include <set>
#include <string>
#include <unordered_map>

typedef struct uv_loop_s uv_loop_t;

namespace uv {

class async;

}

namespace mbgl {

class FileSource;
class Environment;
class Request;

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
    const Shaping getShaping(const std::u32string &string, float maxWidth, float lineHeight,
                             float horizontalAlign, float verticalAlign, float justify,
                             float spacing, const vec2<float> &translate) const;
    void lineWrap(Shaping &shaping, float lineHeight, float maxWidth, float horizontalAlign,
                  float verticalAlign, float justify) const;

private:
    std::map<uint32_t, std::string> bitmaps;
    std::map<uint32_t, GlyphMetrics> metrics;
    std::map<uint32_t, SDFGlyph> sdfs;
    mutable std::mutex mtx;
};

class GlyphPBF {
public:
    using GlyphLoadedCallback = std::function<void(GlyphPBF*)>;

    GlyphPBF(const std::string &glyphURL,
             const std::string &fontStack,
             GlyphRange glyphRange,
             Environment &env,
             const GlyphLoadedCallback& callback);
    ~GlyphPBF();

    void parse(FontStack &stack);
    bool isParsed() const;

private:
    GlyphPBF(const GlyphPBF &) = delete;
    GlyphPBF(GlyphPBF &&) = delete;
    GlyphPBF &operator=(const GlyphPBF &) = delete;
    GlyphPBF &operator=(GlyphPBF &&) = delete;

    std::string data;
    std::atomic_bool parsed;

    Environment& env;
    Request* req = nullptr;

    mutable std::mutex mtx;
};

// Manages Glyphrange PBF loading.
class GlyphStore {
public:
    class Observer {
    public:
        virtual ~Observer() = default;

        virtual void onGlyphRangeLoaded() = 0;
    };

    GlyphStore(uv_loop_t* loop, Environment &);
    ~GlyphStore();

    // Asynchronously request for GlyphRanges and when it gets loaded, notifies the
    // observer subscribed to this object. Successive requests for the same range are
    // going to be discarded. Returns true if a request was made or false if all the
    // GlyphRanges are already available, and thus, no request is performed.
    bool requestGlyphRangesIfNeeded(const std::string &fontStack, const std::set<GlyphRange> &glyphRanges);

    FontStack* getFontStack(const std::string &fontStack);

    void setURL(const std::string &url);

    void setObserver(Observer* observer);

private:
    void emitGlyphRangeLoaded();

    FontStack* createFontStack(const std::string &fontStack);

    std::string glyphURL;
    Environment &env;

    std::unordered_map<std::string, std::map<GlyphRange, std::unique_ptr<GlyphPBF>>> ranges;
    std::mutex rangesMutex;

    std::unordered_map<std::string, std::unique_ptr<FontStack>> stacks;
    std::mutex stacksMutex;

    std::unique_ptr<uv::async> asyncEmitGlyphRangeLoaded;

    Observer* observer;
};

}

#endif
