#ifndef MBGL_TEXT_GLYPH_STORE
#define MBGL_TEXT_GLYPH_STORE

#include <mbgl/text/glyph.hpp>

#include <mbgl/util/exclusive.hpp>

#include <set>
#include <string>
#include <unordered_map>
#include <exception>

typedef struct uv_loop_s uv_loop_t;

namespace uv {
class async;
}

namespace mbgl {

class FontStack;
class GlyphPBF;

// Manages GlyphRange PBF loading.
class GlyphStore {
public:
    class Observer {
    public:
        virtual ~Observer() = default;

        virtual void onGlyphRangeLoaded() = 0;
        virtual void onGlyphRangeLoadingFailed(std::exception_ptr error) = 0;
    };

    GlyphStore(uv_loop_t* loop);
    ~GlyphStore();

    // Asynchronously request for GlyphRanges and when it gets loaded, notifies the
    // observer subscribed to this object. Successive requests for the same range are
    // going to be discarded. Returns true if a request was made or false if all the
    // GlyphRanges are already available, and thus, no request is performed.
    bool requestGlyphRangesIfNeeded(const std::string &fontStack, const std::set<GlyphRange> &glyphRanges);

    util::exclusive<FontStack> getFontStack(const std::string &fontStack);

    void setURL(const std::string &url);

    void setObserver(Observer* observer);

private:
    void emitGlyphRangeLoaded();
    void emitGlyphRangeLoadingFailed();

    util::exclusive<FontStack> createFontStack(const std::string &fontStack);

    std::string glyphURL;

    std::unordered_map<std::string, std::map<GlyphRange, std::unique_ptr<GlyphPBF>>> ranges;
    std::mutex rangesMutex;

    std::unordered_map<std::string, std::unique_ptr<FontStack>> stacks;
    std::mutex stacksMutex;

    std::string errorMessage;
    std::mutex errorMessageMutex;

    std::unique_ptr<uv::async> asyncEmitGlyphRangeLoaded;
    std::unique_ptr<uv::async> asyncEmitGlyphRangeLoadedingFailed;

    Observer* observer;
};

}

#endif
