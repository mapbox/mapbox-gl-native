#include <mbgl/text/glyph_pbf.hpp>

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/text/font_stack.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/pbf.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/thread_context.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/url.hpp>

#include <sstream>

namespace {

void parseGlyphPBF(mbgl::FontStack& stack, const std::string& data) {
    mbgl::pbf glyphs_pbf(reinterpret_cast<const uint8_t *>(data.data()), data.size());

    while (glyphs_pbf.next()) {
        if (glyphs_pbf.tag == 1) { // stacks
            mbgl::pbf fontstack_pbf = glyphs_pbf.message();
            while (fontstack_pbf.next()) {
                if (fontstack_pbf.tag == 3) { // glyphs
                    mbgl::pbf glyph_pbf = fontstack_pbf.message();

                    mbgl::SDFGlyph glyph;

                    while (glyph_pbf.next()) {
                        if (glyph_pbf.tag == 1) { // id
                            glyph.id = glyph_pbf.varint();
                        } else if (glyph_pbf.tag == 2) { // bitmap
                            glyph.bitmap = glyph_pbf.string();
                        } else if (glyph_pbf.tag == 3) { // width
                            glyph.metrics.width = glyph_pbf.varint();
                        } else if (glyph_pbf.tag == 4) { // height
                            glyph.metrics.height = glyph_pbf.varint();
                        } else if (glyph_pbf.tag == 5) { // left
                            glyph.metrics.left = glyph_pbf.svarint();
                        } else if (glyph_pbf.tag == 6) { // top
                            glyph.metrics.top = glyph_pbf.svarint();
                        } else if (glyph_pbf.tag == 7) { // advance
                            glyph.metrics.advance = glyph_pbf.varint();
                        } else {
                            glyph_pbf.skip();
                        }
                    }

                    stack.insert(glyph.id, glyph);
                } else {
                    fontstack_pbf.skip();
                }
            }
        } else {
            glyphs_pbf.skip();
        }
    }
}

}

namespace mbgl {

GlyphPBF::GlyphPBF(GlyphStore* store,
                   const std::string& fontStack,
                   const GlyphRange& glyphRange)
    : parsed(false) {
    // Load the glyph set URL
    std::string url = util::replaceTokens(store->getURL(), [&](const std::string &name) -> std::string {
        if (name == "fontstack") return util::percentEncode(fontStack);
        if (name == "range") return util::toString(glyphRange.first) + "-" + util::toString(glyphRange.second);
        return "";
    });

    auto requestCallback = [this, store, fontStack, url](const Response &res) {
        req = nullptr;

        if (res.status != Response::Successful) {
            std::stringstream message;
            message <<  "Failed to load [" << url << "]: " << res.message;
            emitGlyphPBFLoadingFailed(message.str());
        } else {
            data = res.data;
            parse(store, fontStack, url);
        }
    };

    FileSource* fs = util::ThreadContext::getFileSource();
    req = fs->request({ Resource::Kind::Glyphs, url }, util::RunLoop::getLoop(), requestCallback);
}

GlyphPBF::~GlyphPBF() = default;

void GlyphPBF::parse(GlyphStore* store, const std::string& fontStack, const std::string& url) {
    if (data.empty()) {
        // If there is no data, this means we either haven't
        // received any data.
        return;
    }

    try {
        parseGlyphPBF(**store->getFontStack(fontStack), std::move(data));
    } catch (const std::exception& ex) {
        std::stringstream message;
        message <<  "Failed to parse [" << url << "]: " << ex.what();
        emitGlyphPBFLoadingFailed(message.str());
        return;
    }

    parsed = true;

    emitGlyphPBFLoaded();
}

void GlyphPBF::setObserver(Observer* observer_) {
    observer = observer_;
}

void GlyphPBF::emitGlyphPBFLoaded() {
    if (observer) {
        observer->onGlyphPBFLoaded();
    }
}

void GlyphPBF::emitGlyphPBFLoadingFailed(const std::string& message) {
    if (!observer) {
        return;
    }

    auto error = std::make_exception_ptr(util::GlyphRangeLoadingException(message));
    observer->onGlyphPBFLoadingFailed(error);
}

}
