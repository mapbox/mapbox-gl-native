#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/glyph_set.hpp>
#include <mbgl/geometry/binpack.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/font_stack.hpp>
#include <mbgl/util/exclusive.hpp>
#include <mbgl/util/work_queue.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/gl/object.hpp>

#include <atomic>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <exception>
#include <vector>

namespace mbgl {

class FileSource;
class GlyphPBF;
class GlyphAtlasObserver;

namespace gl {
class Context;
} // namespace gl

class GlyphAtlas : public util::noncopyable {
public:
    GlyphAtlas(Size, FileSource&);
    ~GlyphAtlas();

    util::exclusive<GlyphSet> getGlyphSet(const FontStack&);

    // Returns true if the set of GlyphRanges are available and parsed or false
    // if they are not. For the missing ranges, a request on the FileSource is
    // made and when the glyph if finally parsed, it gets added to the respective
    // GlyphSet and a signal is emitted to notify the observers. This method
    // can be called from any thread.
    bool hasGlyphRanges(const FontStack&, const GlyphRangeSet&);

    void setURL(const std::string &url) {
        glyphURL = url;
    }

    std::string getURL() const {
        return glyphURL;
    }

    void setObserver(GlyphAtlasObserver* observer);

    void addGlyphs(uintptr_t tileUID,
                   const std::u16string& text,
                   const FontStack&,
                   const GlyphSet&,
                   GlyphPositions&);
    void removeGlyphs(uintptr_t tileUID);

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    void bind(gl::Context&, gl::TextureUnit unit);

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload(gl::Context&, gl::TextureUnit unit);

    Size getSize() const;

private:
    void requestGlyphRange(const FontStack&, const GlyphRange&);

    Rect<uint16_t> addGlyph(uintptr_t tileID,
                            const FontStack&,
                            const SDFGlyph&);


    FileSource& fileSource;
    std::string glyphURL;

    std::unordered_map<FontStack, std::map<GlyphRange, std::unique_ptr<GlyphPBF>>, FontStackHash> ranges;
    std::mutex rangesMutex;

    std::unordered_map<FontStack, std::unique_ptr<GlyphSet>, FontStackHash> glyphSets;
    std::mutex glyphSetsMutex;

    util::WorkQueue workQueue;

    GlyphAtlasObserver* observer = nullptr;

    struct GlyphValue {
        GlyphValue(Rect<uint16_t> rect_, uintptr_t id)
            : rect(std::move(rect_)), ids({ id }) {}
        Rect<uint16_t> rect;
        std::unordered_set<uintptr_t> ids;
    };

    std::mutex mtx;
    BinPack<uint16_t> bin;
    std::unordered_map<FontStack, std::map<uint32_t, GlyphValue>, FontStackHash> index;
    const AlphaImage image;
    std::atomic<bool> dirty;
    mbgl::optional<gl::Texture> texture;
};

} // namespace mbgl
