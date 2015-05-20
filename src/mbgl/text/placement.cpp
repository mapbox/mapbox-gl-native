#include <mbgl/text/placement.hpp>
#include <mbgl/geometry/anchor.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/text/placement.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/style_layout.hpp>

#include <mbgl/util/math.hpp>

namespace mbgl {

const float Placement::globalMinScale = 0.5; // underscale by 1 zoom level

struct GlyphInstance {
    explicit GlyphInstance(const vec2<float> &anchor_) : anchor(anchor_) {}
    explicit GlyphInstance(const vec2<float> &anchor_, float offset_, float minScale_, float maxScale_,
                           float angle_)
        : anchor(anchor_), offset(offset_), minScale(minScale_), maxScale(maxScale_), angle(angle_) {}

    const vec2<float> anchor;
    const float offset = 0.0f;
    const float minScale = Placement::globalMinScale;
    const float maxScale = std::numeric_limits<float>::infinity();
    const float angle = 0.0f;
};

typedef std::vector<GlyphInstance> GlyphInstances;

void getSegmentGlyphs(std::back_insert_iterator<GlyphInstances> glyphs, Anchor &anchor,
                      float offset, const std::vector<Coordinate> &line, int segment,
                      int8_t direction, float maxAngle) {
    const bool upsideDown = direction < 0;

    if (offset < 0)
        direction *= -1;

    if (direction > 0)
        segment++;

    vec2<float> newAnchor = anchor;

    if ((int)line.size() <= segment) {
        return;
    }
    vec2<float> end = line[segment];
    float prevscale = std::numeric_limits<float>::infinity();
    float prevAngle = 0.0f;

    offset = std::fabs(offset);

    const float placementScale = anchor.scale;

    while (true) {
        const float dist = util::dist<float>(newAnchor, end);
        if (dist == 0) {
            break;
        }
        const float scale = offset / dist;
        float angle =
            -std::atan2(end.x - newAnchor.x, end.y - newAnchor.y) + direction * M_PI / 2.0f;
        if (upsideDown)
            angle += M_PI;

        // Don't place around sharp corners
        float angleDiff = std::fmod((angle - prevAngle), (2.0f * M_PI));
        if (prevAngle && std::fabs(angleDiff) > maxAngle) {
            anchor.scale = prevscale;
            break;
        }

        glyphs = GlyphInstance{
            /* anchor */ newAnchor,
            /* offset */ static_cast<float>(upsideDown ? M_PI : 0.0),
            /* minScale */ scale,
            /* maxScale */ prevscale,
            /* angle */ static_cast<float>(std::fmod((angle + 2.0 * M_PI), (2.0 * M_PI)))};

        if (scale <= placementScale)
            break;

        newAnchor = end;

        // skip duplicate nodes
        while (newAnchor == end) {
            segment += direction;
            if ((int)line.size() <= segment || segment < 0) {
                anchor.scale = scale;
                return;
            }
            end = line[segment];
        }

        vec2<float> normal = util::normal<float>(newAnchor, end) * dist;
        newAnchor = newAnchor - normal;

        prevscale = scale;
        prevAngle = angle;
    }
}

GlyphBox getMergedBoxes(const GlyphBoxes &glyphs, const Anchor &anchor) {
    // Collision checks between rotating and fixed labels are relatively expensive,
    // so we use one box per label, not per glyph for horizontal labels.

    const float inf = std::numeric_limits<float>::infinity();

    GlyphBox mergedglyphs{/* box */ CollisionRect{inf, inf, -inf, -inf},
                          /* anchor */ anchor,
                          /* minScale */ 0,
                          /* maxScale */ inf,
                          /* padding */ -inf};

    CollisionRect &box = mergedglyphs.box;

    for (const auto& glyph : glyphs) {
        const CollisionRect &gbox = glyph.box;
        box.tl.x = util::min(box.tl.x, gbox.tl.x);
        box.tl.y = util::min(box.tl.y, gbox.tl.y);
        box.br.x = util::max(box.br.x, gbox.br.x);
        box.br.y = util::max(box.br.y, gbox.br.y);
        mergedglyphs.minScale = util::max(mergedglyphs.minScale, glyph.minScale);
        mergedglyphs.padding = util::max(mergedglyphs.padding, glyph.padding);
    }
    // for all horizontal labels, calculate bbox covering all rotated positions
    float x12 = box.tl.x * box.tl.x, y12 = box.tl.y * box.tl.y, x22 = box.br.x * box.br.x,
          y22 = box.br.y * box.br.y,
          diag = std::sqrt(util::max(x12 + y12, x12 + y22, x22 + y12, x22 + y22));

    mergedglyphs.hBox = CollisionRect{-diag, -diag, diag, diag};

    return mergedglyphs;
}

Placement Placement::getIcon(Anchor &anchor, const Rect<uint16_t> &image, float boxScale,
                             const std::vector<Coordinate> &line, const StyleLayoutSymbol &layout) {

    const float padding = 1.0f;
    const float dx = layout.icon.offset[0];
    const float dy = layout.icon.offset[1];
    float x1 = dx - image.originalW / 2.0f - padding;
    float x2 = x1 + image.w;
    float y1 = dy - image.originalH / 2.0f - padding;
    float y2 = y1 + image.h;

    vec2<float> tl{x1, y1};
    vec2<float> tr{x2, y1};
    vec2<float> br{x2, y2};
    vec2<float> bl{x1, y2};

    float angle = layout.icon.rotate * M_PI / 180.0f;
    if (anchor.segment >= 0 && layout.icon.rotation_alignment != RotationAlignmentType::Viewport) {
        const Coordinate &prev = line[anchor.segment];
        angle += std::atan2(anchor.y - prev.y, anchor.x - prev.x);
    }

    if (angle) {
        // Compute the transformation matrix.
        float angle_sin = std::sin(angle);
        float angle_cos = std::cos(angle);
        std::array<float, 4> matrix = {{angle_cos, -angle_sin, angle_sin, angle_cos}};

        tl = tl.matMul(matrix);
        tr = tr.matMul(matrix);
        bl = bl.matMul(matrix);
        br = br.matMul(matrix);

        x1 = util::min(tl.x, tr.x, bl.x, br.x);
        x2 = util::max(tl.x, tr.x, bl.x, br.x);
        y1 = util::min(tl.y, tr.y, bl.y, br.y);
        y2 = util::max(tl.y, tr.y, bl.y, br.y);
    }

    const CollisionRect box{/* x1 */ x1 * boxScale,
                            /* y1 */ y1 * boxScale,
                            /* x2 */ x2 * boxScale,
                            /* y2 */ y2 * boxScale};

    Placement placement;

    placement.boxes.emplace_back(
        /* box */ box,
        /* anchor */ anchor,
        /* minScale */ Placement::globalMinScale,
        /* maxScale */ std::numeric_limits<float>::infinity(),
        /* padding */ layout.icon.padding);

    placement.shapes.emplace_back(
        /* tl */ tl,
        /* tr */ tr,
        /* bl */ bl,
        /* br */ br,
        /* image */ image,
        /* angle */ 0,
        /* anchors */ anchor,
        /* minScale */ Placement::globalMinScale,
        /* maxScale */ std::numeric_limits<float>::infinity());

    placement.minScale = anchor.scale;

    return placement;
}

Placement Placement::getGlyphs(Anchor &anchor, const vec2<float> &origin, const Shaping &shaping,
                               const GlyphPositions &face, float boxScale, bool horizontal,
                               const std::vector<Coordinate> &line,
                               const StyleLayoutSymbol &layout) {
    const float maxAngle = layout.text.max_angle * M_PI / 180;
    const float rotate = layout.text.rotate * M_PI / 180;
    const float padding = layout.text.padding;
    const bool alongLine = layout.text.rotation_alignment != RotationAlignmentType::Viewport;
    const bool keepUpright = layout.text.keep_upright;

    Placement placement;

    const uint32_t glyphPadding = 1;
    const uint32_t buffer = 3 + glyphPadding;

    for (const auto& shape : shaping) {
        auto face_it = face.find(shape.glyph);
        if (face_it == face.end())
            continue;
        const Glyph &glyph = face_it->second;
        const Rect<uint16_t> &rect = glyph.rect;

        if (!glyph)
            continue;

        if (!rect.hasArea())
            continue;

        const float x = (origin.x + shape.x + glyph.metrics.left - buffer + rect.w / 2) * boxScale;

        GlyphInstances glyphInstances;
        if (anchor.segment >= 0 && alongLine) {
            getSegmentGlyphs(std::back_inserter(glyphInstances), anchor, x, line, anchor.segment, 1,
                             maxAngle);
            if (keepUpright)
                getSegmentGlyphs(std::back_inserter(glyphInstances), anchor, x, line,
                                 anchor.segment, -1, maxAngle);

        } else {
            glyphInstances.emplace_back(GlyphInstance{anchor});
        }

        const float x1 = origin.x + shape.x + glyph.metrics.left - buffer;
        const float y1 = origin.y + shape.y - glyph.metrics.top - buffer;
        const float x2 = x1 + glyph.rect.w;
        const float y2 = y1 + glyph.rect.h;

        const vec2<float> otl{x1, y1};
        const vec2<float> otr{x2, y1};
        const vec2<float> obl{x1, y2};
        const vec2<float> obr{x2, y2};

        const CollisionRect obox{boxScale * x1, boxScale * y1, boxScale * x2, boxScale * y2};

        for (const auto& instance : glyphInstances) {
            vec2<float> tl = otl;
            vec2<float> tr = otr;
            vec2<float> bl = obl;
            vec2<float> br = obr;

            CollisionRect box = obox;

            // Clamp to -90/+90 degrees
            const float angle = instance.angle + rotate;

            if (angle) {
                // Compute the transformation matrix.
                float angle_sin = std::sin(angle);
                float angle_cos = std::cos(angle);
                std::array<float, 4> matrix = {{angle_cos, -angle_sin, angle_sin, angle_cos}};

                tl = tl.matMul(matrix);
                tr = tr.matMul(matrix);
                bl = bl.matMul(matrix);
                br = br.matMul(matrix);
            }

            // Prevent label from extending past the end of the line
            const float glyphMinScale = std::max(instance.minScale, anchor.scale);

            // Remember the glyph for later insertion.
            placement.shapes.emplace_back(
                tl, tr, bl, br, rect,
                float(std::fmod((anchor.angle + rotate + instance.offset + 2 * M_PI), (2 * M_PI))),
                instance.anchor, glyphMinScale, instance.maxScale);

            if (!instance.offset) { // not a flipped glyph
                if (angle) {
                    // Calculate the rotated glyph's bounding box offsets from the anchor point.
                    box = CollisionRect{boxScale * util::min(tl.x, tr.x, bl.x, br.x),
                                        boxScale * util::min(tl.y, tr.y, bl.y, br.y),
                                        boxScale * util::max(tl.x, tr.x, bl.x, br.x),
                                        boxScale * util::max(tl.y, tr.y, bl.y, br.y)};
                }
                placement.boxes.emplace_back(box, instance.anchor, glyphMinScale, instance.maxScale, padding);
            }
        }
    }

    // TODO avoid creating the boxes in the first place?
    if (horizontal)
        placement.boxes = {getMergedBoxes(placement.boxes, anchor)};

    const float minPlacementScale = anchor.scale;
    placement.minScale = std::numeric_limits<float>::infinity();
    for (const auto& box : placement.boxes) {
        placement.minScale = util::min(placement.minScale, box.minScale);
    }
    placement.minScale = util::max(minPlacementScale, Placement::globalMinScale);

    return placement;
}
}
