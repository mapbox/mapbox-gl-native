#include <mbgl/text/glyph_pbf.hpp>
#include <mbgl/text/font_stack.hpp>

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/util/pbf.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/url.hpp>

#include <sstream>

namespace mbgl {

GlyphPBF::GlyphPBF(const std::string& glyphURL,
                   const std::string& fontStack,
                   GlyphRange glyphRange,
                   const GlyphLoadedCallback& successCallback,
                   const GlyphLoadingFailedCallback& failureCallback)
    : parsed(false) {
    // Load the glyph set URL
    url = util::replaceTokens(glyphURL, [&](const std::string &name) -> std::string {
        if (name == "fontstack") return util::percentEncode(fontStack);
        if (name == "range") return util::toString(glyphRange.first) + "-" + util::toString(glyphRange.second);
        return "";
    });

    // The prepare call jumps back to the main thread.
    FileSource* fs = util::ThreadContext::getFileSource();
    req = fs->request({ Resource::Kind::Glyphs, url }, util::RunLoop::getLoop(), [&, successCallback, failureCallback](const Response &res) {
        req = nullptr;

        if (res.status != Response::Successful) {
            std::stringstream message;
            message <<  "Failed to load [" << url << "]: " << res.message;
            failureCallback(message.str());
        } else {
            // Transfer the data to the GlyphSet and signal its availability.
            // Once it is available, the caller will need to call parse() to actually
            // parse the data we received. We are not doing this here since this callback is being
            // called from another (unknown) thread.
            data = res.data;
            successCallback(this);
        }
    });
}

GlyphPBF::~GlyphPBF() {
    if (req) {
        util::ThreadContext::getFileSource()->cancel(req);
    }
}

void GlyphPBF::parse(FontStack &stack) {
    if (!data.size()) {
        // If there is no data, this means we either haven't received any data, or
        // we have already parsed the data.
        return;
    }

    // Parse the glyph PBF
    pbf glyphs_pbf(reinterpret_cast<const uint8_t *>(data.data()), data.size());

    while (glyphs_pbf.next()) {
        if (glyphs_pbf.tag == 1) { // stacks
            pbf fontstack_pbf = glyphs_pbf.message();
            while (fontstack_pbf.next()) {
                if (fontstack_pbf.tag == 3) { // glyphs
                    pbf glyph_pbf = fontstack_pbf.message();

                    SDFGlyph glyph;

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

    data.clear();

    parsed = true;
}

bool GlyphPBF::isParsed() const {
    return parsed;
}

}
