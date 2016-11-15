#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/geometry/debug_font_data.hpp>
#include <mbgl/util/string.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace mbgl {

gl::VertexVector<FillVertex, gl::Lines>
buildTextVertices(const OverscaledTileID& id,
                  const bool renderable,
                  const bool complete,
                  optional<Timestamp> modified,
                  optional<Timestamp> expires,
                  MapDebugOptions debugMode) {
    gl::VertexVector<FillVertex, gl::Lines> textLines;

    auto addText = [&] (const std::string& text, double left, double baseline, double scale) {
        for (uint8_t c : text) {
            if (c < 32 || c >= 127)
                continue;

            optional<Point<int16_t>> prev;

            const glyph& glyph = simplex[c - 32];
            for (int32_t j = 0; j < glyph.length; j += 2) {
                if (glyph.data[j] == -1 && glyph.data[j + 1] == -1) {
                    prev = {};
                } else {
                    Point<int16_t> p {
                        int16_t(::round(left + glyph.data[j] * scale)),
                        int16_t(::round(baseline - glyph.data[j + 1] * scale))
                    };

                    if (prev) {
                        textLines.emplace_back(FillAttributes::vertex(*prev),
                                               FillAttributes::vertex(p));
                    }

                    prev = p;
                }
            }

            left += glyph.width * scale;
        }
    };

    double baseline = 200;
    if (debugMode & MapDebugOptions::ParseStatus) {
        const std::string text = util::toString(id) + " - " +
                                 (complete ? "complete" : renderable ? "renderable" : "pending");
        addText(text, 50, baseline, 5);
        baseline += 200;
    }

    if (debugMode & MapDebugOptions::Timestamps && modified && expires) {
        const std::string modifiedText = "modified: " + util::iso8601(*modified);
        addText(modifiedText, 50, baseline, 5);

        const std::string expiresText = "expires: " + util::iso8601(*expires);
        addText(expiresText, 50, baseline + 200, 5);
    }

    return textLines;
}

DebugBucket::DebugBucket(const OverscaledTileID& id,
                         const bool renderable_,
                         const bool complete_,
                         optional<Timestamp> modified_,
                         optional<Timestamp> expires_,
                         MapDebugOptions debugMode_,
                         gl::Context& context)
    : renderable(renderable_),
      complete(complete_),
      modified(std::move(modified_)),
      expires(std::move(expires_)),
      debugMode(debugMode_),
      vertexBuffer(context.createVertexBuffer(buildTextVertices(id, renderable_, complete_, modified_, expires_, debugMode_))),
      segments(vertexBuffer) {
}

} // namespace mbgl
