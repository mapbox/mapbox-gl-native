#include <mbgl/text/glyph_store.hpp>

#include <mbgl/map/environment.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/utf.hpp>
#include <mbgl/util/pbf.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <algorithm>

namespace mbgl {


void FontStack::insert(uint32_t id, const SDFGlyph &glyph) {
    std::lock_guard<std::mutex> lock(mtx);
    metrics.emplace(id, glyph.metrics);
    bitmaps.emplace(id, glyph.bitmap);
    sdfs.emplace(id, glyph);
}

const std::map<uint32_t, GlyphMetrics> &FontStack::getMetrics() const {
    std::lock_guard<std::mutex> lock(mtx);
    return metrics;
}

const std::map<uint32_t, SDFGlyph> &FontStack::getSDFs() const {
    std::lock_guard<std::mutex> lock(mtx);
    return sdfs;
}

const Shaping FontStack::getShaping(const std::u32string &string, const float maxWidth,
                                    const float lineHeight, const float horizontalAlign,
                                    const float verticalAlign, const float justify,
                                    const float spacing, const vec2<float> &translate) const {
    std::lock_guard<std::mutex> lock(mtx);

    Shaping shaping;

    int32_t x = std::round(translate.x * 24); // one em
    const int32_t y = std::round(translate.y * 24); // one em

    // Loop through all characters of this label and shape.
    for (uint32_t chr : string) {
        shaping.emplace_back(chr, x, y);
        auto metric = metrics.find(chr);
        if (metric != metrics.end()) {
            x += metric->second.advance + spacing;
        }
    }

    if (!shaping.size())
        return shaping;

    lineWrap(shaping, lineHeight, maxWidth, horizontalAlign, verticalAlign, justify);

    return shaping;
}

void align(Shaping &shaping, const float justify, const float horizontalAlign,
           const float verticalAlign, const uint32_t maxLineLength, const float lineHeight,
           const uint32_t line) {
    const float shiftX = (justify - horizontalAlign) * maxLineLength;
    const float shiftY = (-verticalAlign * (line + 1) + 0.5) * lineHeight;

    for (auto& glyph : shaping) {
        glyph.x += shiftX;
        glyph.y += shiftY;
    }
}

void justifyLine(Shaping &shaping, const std::map<uint32_t, GlyphMetrics> &metrics, uint32_t start,
                 uint32_t end, float justify) {
    PositionedGlyph &glyph = shaping[end];
    auto metric = metrics.find(glyph.glyph);
    if (metric != metrics.end()) {
        const uint32_t lastAdvance = metric->second.advance;
        const float lineIndent = float(glyph.x + lastAdvance) * justify;

        for (uint32_t j = start; j <= end; j++) {
            shaping[j].x -= lineIndent;
        }
    }
}

void FontStack::lineWrap(Shaping &shaping, const float lineHeight, const float maxWidth,
                         const float horizontalAlign, const float verticalAlign,
                         const float justify) const {
    uint32_t lastSafeBreak = 0;

    uint32_t lengthBeforeCurrentLine = 0;
    uint32_t lineStartIndex = 0;
    uint32_t line = 0;

    uint32_t maxLineLength = 0;

    if (maxWidth) {
        for (uint32_t i = 0; i < shaping.size(); i++) {
            PositionedGlyph &shape = shaping[i];

            shape.x -= lengthBeforeCurrentLine;
            shape.y += lineHeight * line;

            if (shape.x > maxWidth && lastSafeBreak > 0) {

                uint32_t lineLength = shaping[lastSafeBreak + 1].x;
                maxLineLength = util::max(lineLength, maxLineLength);

                for (uint32_t k = lastSafeBreak + 1; k <= i; k++) {
                    shaping[k].y += lineHeight;
                    shaping[k].x -= lineLength;
                }

                if (justify) {
                    justifyLine(shaping, metrics, lineStartIndex, lastSafeBreak - 1, justify);
                }

                lineStartIndex = lastSafeBreak + 1;
                lastSafeBreak = 0;
                lengthBeforeCurrentLine += lineLength;
                line++;
            }

            if (shape.glyph == 32) {
                lastSafeBreak = i;
            }
        }
    }

    if (!maxLineLength) maxLineLength = shaping.back().x;

    justifyLine(shaping, metrics, lineStartIndex, uint32_t(shaping.size()) - 1, justify);
    align(shaping, justify, horizontalAlign, verticalAlign, maxLineLength, lineHeight, line);
}

GlyphPBF::GlyphPBF(const std::string& glyphURL,
                   const std::string& fontStack,
                   GlyphRange glyphRange,
                   Environment& env_,
                   const GlyphLoadedCallback& callback)
    : parsed(false), env(env_) {
    // Load the glyph set URL
    std::string url = util::replaceTokens(glyphURL, [&](const std::string &name) -> std::string {
        if (name == "fontstack") return util::percentEncode(fontStack);
        if (name == "range") return util::toString(glyphRange.first) + "-" + util::toString(glyphRange.second);
        return "";
    });

    // The prepare call jumps back to the main thread.
    req = env.request({ Resource::Kind::Glyphs, url }, [&, url, callback](const Response &res) {
        req = nullptr;

        if (res.status != Response::Successful) {
            // Something went wrong with loading the glyph pbf.
            const std::string msg = std::string { "[ERROR] failed to load glyphs: " } + url + " message: " + res.message;
            Log::Error(Event::HttpRequest, msg);
        } else {
            // Transfer the data to the GlyphSet and signal its availability.
            // Once it is available, the caller will need to call parse() to actually
            // parse the data we received. We are not doing this here since this callback is being
            // called from another (unknown) thread.
            data = res.data;
            parsed = true;
            callback(this);
        }
    });
}

GlyphPBF::~GlyphPBF() {
    if (req) {
        env.cancelRequest(req);
    }
}

void GlyphPBF::parse(FontStack &stack) {
    std::lock_guard<std::mutex> lock(mtx);

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
}

bool GlyphPBF::isParsed() const {
    return parsed;
}

GlyphStore::GlyphStore(uv_loop_t* loop, Environment& env_)
    : env(env_),
      asyncEmitGlyphRangeLoaded(util::make_unique<uv::async>(loop, [this] { emitGlyphRangeLoaded(); })),
      observer(nullptr) {
    asyncEmitGlyphRangeLoaded->unref();
}

GlyphStore::~GlyphStore() {
    observer = nullptr;
}

void GlyphStore::setURL(const std::string &url) {
    glyphURL = url;
}

bool GlyphStore::requestGlyphRangesIfNeeded(const std::string& fontStack,
                                            const std::set<GlyphRange>& glyphRanges) {
    bool requestIsNeeded = false;

    if (glyphRanges.empty()) {
        return requestIsNeeded;
    }

    auto callback = [this, fontStack](GlyphPBF* glyph) {
        glyph->parse(*createFontStack(fontStack));
        asyncEmitGlyphRangeLoaded->send();
    };

    std::lock_guard<std::mutex> lock(rangesMutex);
    auto& rangeSets = ranges[fontStack];

    for (const auto& range : glyphRanges) {
        const auto& rangeSets_it = rangeSets.find(range);
        if (rangeSets_it == rangeSets.end()) {
            auto glyph = util::make_unique<GlyphPBF>(glyphURL, fontStack, range, env, callback);
            rangeSets.emplace(range, std::move(glyph));
            requestIsNeeded = true;
            continue;
        }

        if (!rangeSets_it->second->isParsed()) {
            requestIsNeeded = true;
        }
    }

    return requestIsNeeded;
}

FontStack* GlyphStore::createFontStack(const std::string &fontStack) {
    std::lock_guard<std::mutex> lock(stacksMutex);

    auto stack_it = stacks.find(fontStack);
    if (stack_it == stacks.end()) {
        stack_it = stacks.emplace(fontStack, util::make_unique<FontStack>()).first;
    }

    return stack_it->second.get();
}

FontStack* GlyphStore::getFontStack(const std::string &fontStack) {
    std::lock_guard<std::mutex> lock(stacksMutex);

    const auto& stack_it = stacks.find(fontStack);
    if (stack_it == stacks.end()) {
        return nullptr;
    }

    return stack_it->second.get();
}

void GlyphStore::setObserver(Observer* observer_) {
    observer = observer_;
}

void GlyphStore::emitGlyphRangeLoaded() {
    if (observer) {
        observer->onGlyphRangeLoaded();
    }
}

}
