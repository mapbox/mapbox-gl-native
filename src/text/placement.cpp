#include <llmr/text/placement.hpp>
#include <llmr/map/vector_tile.hpp>
#include <llmr/geometry/interpolate.hpp>
#include <llmr/style/bucket_description.hpp>
#include <llmr/renderer/text_bucket.hpp>
#include <llmr/util/math.hpp>
#include <llmr/util/constants.hpp>

#include <algorithm>

using namespace llmr;

const int Placement::tileExtent = 4096;
const int Placement::glyphSize =
    24; // size in pixels of this glyphs in the tile
const float Placement::minScale = 0.5; // underscale by 1 zoom level

Placement::Placement(int8_t zoom)
    : zoom(zoom),
      zOffset(log(256.0 / util::tileSize) / log(2)),

      // Calculate the maximum scale we can go down in our fake-3d rtree so that
      // placement still makes sense. This is calculated so that the minimum
      // placement zoom can be at most 25.5 (we use an unsigned integer x10 to
      // store the minimum zoom).
      //
      // We don't want to place labels all the way to 25.5. This lets too many
      // glyphs be placed, slowing down collision checking. Only place labels if
      // they will show up within the intended zoom range of the tile.
      // TODO make this not hardcoded to 3
      maxPlacementScale(std::exp(log(2) * util::min((25.5 - zoom), 3.0))) {}

bool byScale(const Anchor &a, const Anchor &b) { return a.scale < b.scale; }

static const Glyph null_glyph;

inline const Glyph &getGlyph(const GlyphPlacement &placed,
                             const GlyphPositions &face) {
    auto it = face.find(placed.glyph);
    if (it != face.end()) {
        return it->second;
    } else {
        fprintf(stderr, "glyph %d does not exist\n", placed.glyph);
    }

    return null_glyph;
}

struct GlyphInstance {
    explicit GlyphInstance(const vec2<float> &anchor) : anchor(anchor) {}
    explicit GlyphInstance(const vec2<float> &anchor, float offset,
                           float minScale, float maxScale, float angle)
        : anchor(anchor),
          offset(offset),
          minScale(minScale),
          maxScale(maxScale),
          angle(angle) {}

    const vec2<float> anchor;
    const float offset = 0.0f;
    const float minScale = Placement::minScale;
    const float maxScale = std::numeric_limits<float>::infinity();
    const float angle = 0.0f;
};

typedef std::vector<GlyphInstance> GlyphInstances;

void getSegmentGlyphs(std::back_insert_iterator<GlyphInstances> glyphs,
                      Anchor &anchor, float offset,
                      const std::vector<Coordinate> &line, int segment,
                      int8_t direction, float maxAngleDelta) {
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
        const float scale = offset / dist;
        float angle = -std::atan2(end.x - newAnchor.x, end.y - newAnchor.y) +
                      direction * M_PI / 2.0f;
        if (upsideDown)
            angle += M_PI;

        // Don't place around sharp corners
        float angleDiff = std::fmod((angle - prevAngle), (2.0f * M_PI));
        if (prevAngle && std::fabs(angleDiff) > maxAngleDelta) {
            anchor.scale = prevscale;
            break;
        }

        glyphs = GlyphInstance{
            /* anchor */ newAnchor,
            /* offset */ static_cast<float>(upsideDown ? M_PI : 0.0),
            /* minScale */ scale,
            /* maxScale */ prevscale,
            /* angle */ static_cast<float>(
                std::fmod((angle + 2.0 * M_PI), (2.0 * M_PI)))};

        if (scale <= placementScale)
            break;

        newAnchor = end;

        // skip duplicate nodes
        while (newAnchor == end) {
            segment += direction;
            if ((int)line.size() <= segment) {
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

void getGlyphs(PlacedGlyphs &glyphs, GlyphBoxes &boxes,
                       Anchor &anchor, vec2<float> origin, const Shaping &shaping,
                       const GlyphPositions &face, float fontScale,
                       bool horizontal, const std::vector<Coordinate> &line,
                       float maxAngleDelta, float rotate) {
    // The total text advance is the width of this label.

    // TODO: allow setting an alignment
    // var alignment = 'center';
    // if (alignment == 'center') {
    //     origin.x -= advance / 2;
    // } else if (alignment == 'right') {
    //     origin.x -= advance;
    // }

    const uint32_t buffer = 3;

    for (const GlyphPlacement &placed : shaping) {
        const Glyph &glyph = getGlyph(placed, face);
        if (!glyph) {
            // This glyph is empty and doesn't have any pixels that we'd need to
            // show.
            continue;
        }

        float x =
            (origin.x + placed.x + glyph.metrics.left - buffer + glyph.rect.w / 2) *
            fontScale;

        GlyphInstances glyphInstances;
        if (anchor.segment >= 0) {
            getSegmentGlyphs(std::back_inserter(glyphInstances), anchor, x,
                             line, anchor.segment, 1, maxAngleDelta);
            getSegmentGlyphs(std::back_inserter(glyphInstances), anchor, x,
                             line, anchor.segment, -1, maxAngleDelta);

        } else {

            glyphInstances.emplace_back(GlyphInstance{anchor});
        }

        const float x1 = origin.x + placed.x + glyph.metrics.left - buffer;
        const float y1 = origin.y + placed.y - glyph.metrics.top - buffer;
        const float x2 = x1 + glyph.rect.w;
        const float y2 = y1 + glyph.rect.h;

        const vec2<float> otl{x1, y1};
        const vec2<float> otr{x2, y1};
        const vec2<float> obl{x1, y2};
        const vec2<float> obr{x2, y2};

        const CollisionRect obox{fontScale *x1, fontScale *y1,
                                 fontScale *x2, fontScale *y2};

        for (const GlyphInstance &instance : glyphInstances) {
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
                std::array<float, 4> matrix = {
                    {angle_cos, -angle_sin, angle_sin, angle_cos}};

                tl = tl.matMul(matrix);
                tr = tr.matMul(matrix);
                bl = bl.matMul(matrix);
                br = br.matMul(matrix);

                // Calculate the rotated glyph's bounding box offsets from the
                // anchor point.
                box = CollisionRect{
                    fontScale * util::min(tl.x, tr.x, bl.x, br.x),
                    fontScale * util::min(tl.y, tr.y, bl.y, br.y),
                    fontScale * util::max(tl.x, tr.x, bl.x, br.x),
                    fontScale * util::max(tl.y, tr.y, bl.y, br.y)};
            }

            GlyphBox glyphBox = GlyphBox{
                box,
                // Prevent label from extending past the end of the line
                util::max(instance.minScale, anchor.scale),
                instance.maxScale,
                instance.anchor,
                horizontal};

            // Remember the glyph for later insertion.
            glyphs.emplace_back(PlacedGlyph{
                tl, tr, bl, br, glyph.rect,
                static_cast<float>(
                    std::fmod((anchor.angle + rotate + instance.offset + 2 * M_PI), (2 * M_PI))),
                glyphBox});

            if (instance.offset == 0.0f) {
                boxes.emplace_back(glyphBox);
            }
        }
    }
}

void Placement::addFeature(TextBucket& bucket,
                           const std::vector<Coordinate> &line,
                           const BucketGeometryDescription &info,
                           const GlyphPositions &face,
                           const Shaping &shaping) {

    const bool horizontal = info.path == TextPathType::Horizontal;
    const float padding = info.padding;
    const float maxAngleDelta = info.maxAngleDelta;
    const float textMinDistance = info.textMinDistance;
    const float rotate = info.rotate;
    const float fontScale =
        (tileExtent / util::tileSize) / (glyphSize / info.size);

    Anchors anchors;

    if (line.size() == 1) {
        // Point labels
        anchors = Anchors{{Anchor{
            static_cast<float>(line[0].x), static_cast<float>(line[0].y),
            0,                             minScale}}};

    } else {
        // Line labels
        anchors = interpolate(line, textMinDistance, minScale);

        // Sort anchors by segment so that we can start placement with
        // the anchors that can be shown at the lowest zoom levels.
        std::sort(anchors.begin(), anchors.end(), byScale);
    }

    for (Anchor anchor : anchors) {
        PlacedGlyphs glyphs;
        GlyphBoxes boxes;

        getGlyphs(glyphs, boxes, anchor, info.translate, shaping, face, fontScale, horizontal,
                      line, maxAngleDelta, rotate);
        PlacementProperty place =
            collision.place(boxes, anchor, anchor.scale, maxPlacementScale,
                            padding, horizontal, info.alwaysVisible);
        if (place) {
            bucket.addGlyphs(glyphs, place.zoom, place.rotationRange, zoom - zOffset);
        }
    }
}
