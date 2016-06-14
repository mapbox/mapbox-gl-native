#pragma once

#include <mbgl/geometry/binpack.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/util/atomic.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/object_store.hpp>

#include <string>
#include <set>
#include <unordered_map>
#include <mutex>

namespace mbgl {

class GlyphAtlas : public util::noncopyable {
public:
    GlyphAtlas(uint16_t width, uint16_t height);
    ~GlyphAtlas();

    void addGlyphs(uintptr_t tileUID,
                   const std::u32string& text,
                   const FontStack&,
                   const GlyphSet&,
                   GlyphPositions&);
    void removeGlyphs(uintptr_t tileUID);

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    void bind(gl::ObjectStore&);

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload(gl::ObjectStore&);

    const GLsizei width;
    const GLsizei height;

private:
    struct GlyphValue {
        GlyphValue(const Rect<uint16_t>& rect_, uintptr_t id)
            : rect(rect_), ids({ id }) {}
        Rect<uint16_t> rect;
        std::set<uintptr_t> ids;
    };

    Rect<uint16_t> addGlyph(uintptr_t tileID,
                            const FontStack&,
                            const SDFGlyph&);

    std::mutex mtx;
    BinPack<uint16_t> bin;
    std::unordered_map<FontStack, std::map<uint32_t, GlyphValue>, FontStackHash> index;
    const std::unique_ptr<uint8_t[]> data;
    util::Atomic<bool> dirty;
    mbgl::optional<gl::UniqueTexture> texture;
};

} // namespace mbgl
