#include <mbgl/renderer/buckets/debug_bucket.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/geometry/debug_font_data.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/string.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace mbgl {

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
      debugMode(debugMode_) {

    gl::VertexVector<FillLayoutVertex> vertices;
    gl::IndexVector<gl::Lines> indices;

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

                    vertices.emplace_back(FillProgram::layoutVertex(p));

                    if (prev) {
                        indices.emplace_back(vertices.vertexSize() - 2,
                                             vertices.vertexSize() - 1);
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

    segments.emplace_back(0, 0, vertices.vertexSize(), indices.indexSize());

    vertexBuffer = context.createVertexBuffer(std::move(vertices));
    indexBuffer = context.createIndexBuffer(std::move(indices));
}

} // namespace mbgl
