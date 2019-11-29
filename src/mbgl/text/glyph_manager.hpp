#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/glyph_manager_observer.hpp>
#include <mbgl/text/glyph_range.hpp>
#include <mbgl/text/local_glyph_rasterizer.hpp>
#include <mbgl/util/font_stack.hpp>
#include <mbgl/util/immutable.hpp>

#include <string>
#include <unordered_map>

namespace mbgl {

class FileSource;
class AsyncRequest;
class Response;

class GlyphRequestor {
public:
    virtual void onGlyphsAvailable(GlyphMap) = 0;

protected:
    virtual ~GlyphRequestor() = default;
};

class GlyphManager {
public:
    GlyphManager(const GlyphManager&) = delete;
    GlyphManager& operator=(const GlyphManager&) = delete;
    explicit GlyphManager(std::unique_ptr<LocalGlyphRasterizer> = std::make_unique<LocalGlyphRasterizer>(optional<std::string>()));
    ~GlyphManager();

    // Workers send a `getGlyphs` message to the main thread once they have determined
    // their `GlyphDependencies`. If all glyphs are already locally available, GlyphManager
    // will provide them to the requestor immediately. Otherwise, it makes a request on the
    // FileSource is made for each range needed, and notifies the observer when all are
    // complete.
    void getGlyphs(GlyphRequestor&, GlyphDependencies, FileSource&);
    void removeRequestor(GlyphRequestor&);

    void setURL(const std::string& url) {
        glyphURL = url;
    }

    void setObserver(GlyphManagerObserver*);

    // Remove glyphs for all but the supplied font stacks.
    void evict(const std::set<FontStack>&);

private:
    Glyph generateLocalSDF(const FontStack& fontStack, GlyphID glyphID);
    std::string glyphURL;

    struct GlyphRequest {
        bool parsed = false;
        std::unique_ptr<AsyncRequest> req;
        std::unordered_map<GlyphRequestor*, std::shared_ptr<GlyphDependencies>> requestors;
    };

    struct Entry {
        std::map<GlyphRange, GlyphRequest> ranges;
        std::map<GlyphID, Immutable<Glyph>> glyphs;
        optional<int32_t> ascender;
        optional<int32_t> descender;
    };

    std::unordered_map<FontStack, Entry, FontStackHasher> entries;

    void requestRange(GlyphRequest&, const FontStack&, const GlyphRange&, FileSource& fileSource);
    void processResponse(const Response&, const FontStack&, const GlyphRange&);
    void notify(GlyphRequestor&, const GlyphDependencies&);
    
    GlyphManagerObserver* observer = nullptr;
    
    std::unique_ptr<LocalGlyphRasterizer> localGlyphRasterizer;
};

} // namespace mbgl
