#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/glyph_atlas_observer.hpp>
#include <mbgl/text/glyph_range.hpp>
#include <mbgl/text/glyph_set.hpp>
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
class GlyphPBF;

namespace gl {
class Context;
} // namespace gl

class GlyphRequestor {
public:
    virtual void onGlyphsAvailable(GlyphPositionMap) = 0;
};
    
class GlyphAtlas : public util::noncopyable, public GlyphAtlasObserver {
public:
    GlyphAtlas(Size, FileSource&);
    ~GlyphAtlas();

    GlyphSet& getGlyphSet(const FontStack&);
    
    // Workers send a `getGlyphs` message to the main thread once they have determined
    // which glyphs they will need. Invoking this method will increment reference
    // counts for all the glyphs in `GlyphDependencies`. If all glyphs are already
    // locally available, the observer will be notified that the glyphs are available
    // immediately. Otherwise, a request on the FileSource is made, and when all glyphs
    // are parsed and added to the atlas, the observer will be notified.
    // Workers are given a copied 'GlyphPositions' map to use for placing their glyphs.
    // The positions specified in this object are guaranteed to be
    // valid for the lifetime of the tile.
    void getGlyphs(GlyphRequestor& requestor, GlyphDependencies glyphs);

    void setURL(const std::string &url) {
        glyphURL = url;
    }

    std::string getURL() const {
        return glyphURL;
    }

    void setObserver(GlyphAtlasObserver* observer);

    void removeGlyphs(GlyphRequestor&);

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    void bind(gl::Context&, gl::TextureUnit unit);

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload(gl::Context&, gl::TextureUnit unit);

    Size getSize() const;
    
    virtual void onGlyphsLoaded(const FontStack&, const GlyphRange&);
    virtual void onGlyphsError(const FontStack&, const GlyphRange&, std::exception_ptr);
    
    friend class ::GlyphAtlasTest;

private:
    void addGlyphs(GlyphRequestor& requestor, const GlyphDependencies& glyphDependencies);

    // Only used by GlyphAtlasTest
    bool hasGlyphRanges(const FontStack&, const GlyphRangeSet& ranges) const;
    bool hasGlyphRange(const FontStack&, const GlyphRange& range) const;

    void addGlyph(GlyphRequestor& requestor, const FontStack&, const SDFGlyph&);
    
    FileSource& fileSource;
    std::string glyphURL;

    struct GlyphValue {
        GlyphValue(Rect<uint16_t> rect_, GlyphRequestor* id)
            : rect(std::move(rect_)), ids({ id }) {}
        Rect<uint16_t> rect;
        std::unordered_set<GlyphRequestor*> ids;
    };

    struct Entry {
        std::map<GlyphRange, GlyphPBF> ranges;
        GlyphSet glyphSet;
        std::map<uint32_t, GlyphValue> glyphValues;
    };

    std::unordered_map<FontStack, Entry, FontStackHash> entries;
    
    void removeGlyphValues(GlyphRequestor& requestor, std::map<uint32_t, GlyphValue>& face);
    void removePendingRanges(GlyphRequestor& requestor, std::map<GlyphRange, GlyphPBF>& ranges);

    GlyphAtlasObserver* observer = nullptr;

    BinPack<uint16_t> bin;
    AlphaImage image;
    bool dirty;
    mbgl::optional<gl::Texture> texture;
};

} // namespace mbgl
