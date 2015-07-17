#ifndef MBGL_TEXT_GLYPH_STORE
#define MBGL_TEXT_GLYPH_STORE

#include <mbgl/text/glyph.hpp>
#include <mbgl/util/exclusive.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/work_queue.hpp>

#include <exception>
#include <set>
#include <string>
#include <unordered_map>

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

    GlyphStore();
    ~GlyphStore();

    util::exclusive<FontStack> getFontStack(const std::string& fontStack);

    bool hasGlyphRanges(const std::string& fontStackName, const std::set<GlyphRange>& glyphRanges);

    void setURL(const std::string &url) {
        glyphURL = url;
    }

    void setObserver(Observer* observer);

private:
    void emitGlyphRangeLoaded();
    void emitGlyphRangeLoadingFailed(const std::string& message);

    util::exclusive<FontStack> createFontStack(const std::string &fontStack);
    void requestGlyphRange(const std::string& fontStackName, const GlyphRange& range);

    std::string glyphURL;

    std::unordered_map<std::string, std::map<GlyphRange, std::unique_ptr<GlyphPBF>>> ranges;
    std::mutex rangesMutex;

    std::unordered_map<std::string, std::unique_ptr<FontStack>> stacks;
    std::mutex stacksMutex;

    util::WorkQueue workQueue;

    Observer* observer = nullptr;
};

}

#endif
