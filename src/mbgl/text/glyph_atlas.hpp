#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/glyph_atlas_observer.hpp>
#include <mbgl/text/glyph_range.hpp>
#include <mbgl/geometry/binpack.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/font_stack.hpp>
#include <mbgl/util/work_queue.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/gl/object.hpp>

#include <string>
#include <unordered_set>
#include <unordered_map>

class GlyphAtlasTest;

namespace mbgl {

class FileSource;
class AsyncRequest;
class Response;

namespace gl {
class Context;
} // namespace gl

class GlyphRequestor {
public:
    virtual ~GlyphRequestor() = default;
    virtual void onGlyphsAvailable(GlyphPositionMap) = 0;
};
    
class GlyphAtlas : public util::noncopyable {
public:
    GlyphAtlas(Size, FileSource&);
    ~GlyphAtlas();

    // Workers send a `getGlyphs` message to the main thread once they have determined
    // which glyphs they will need. Invoking this method will increment reference
    // counts for all the glyphs in `GlyphDependencies`. If all glyphs are already
    // locally available, the observer will be notified that the glyphs are available
    // immediately. Otherwise, a request on the FileSource is made, and when all glyphs
    // are parsed and added to the atlas, the observer will be notified.
    // Workers are given a copied 'GlyphPositions' map to use for placing their glyphs.
    // The positions specified in this object are guaranteed to be
    // valid for the lifetime of the tile.
    void getGlyphs(GlyphRequestor&, GlyphDependencies);
    void removeGlyphs(GlyphRequestor&);

    void setURL(const std::string& url) {
        glyphURL = url;
    }

    void setObserver(GlyphAtlasObserver*);

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    void bind(gl::Context&, gl::TextureUnit unit);

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload(gl::Context&, gl::TextureUnit unit);

    Size getSize() const;

private:
    FileSource& fileSource;
    std::string glyphURL;

    struct GlyphValue {
        AlphaImage bitmap;
        GlyphMetrics metrics;
        optional<Rect<uint16_t>> rect;
        std::unordered_set<GlyphRequestor*> ids;
    };

    struct GlyphRequest {
        bool parsed = false;
        std::unique_ptr<AsyncRequest> req;
        std::unordered_map<GlyphRequestor*, std::shared_ptr<GlyphDependencies>> requestors;
    };

    struct Entry {
        std::map<GlyphRange, GlyphRequest> ranges;
        std::map<GlyphID, GlyphValue> glyphs;
    };

    std::unordered_map<FontStack, Entry, FontStackHash> entries;

    GlyphRequest& requestRange(Entry&, const FontStack&, const GlyphRange&);
    void processResponse(const Response&, const FontStack&, const GlyphRange&);

    void addGlyphs(GlyphRequestor&, const GlyphDependencies&);
    Rect<uint16_t> addGlyph(GlyphValue&);

    void removeGlyphValues(GlyphRequestor&, std::map<GlyphID, GlyphValue>&);
    void removePendingRanges(GlyphRequestor&, std::map<GlyphRange, GlyphRequest>&);

    GlyphAtlasObserver* observer = nullptr;

    BinPack<uint16_t> bin;
    AlphaImage image;
    bool dirty;
    mbgl::optional<gl::Texture> texture;
};

} // namespace mbgl
