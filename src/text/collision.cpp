#include <mbgl/text/collision.hpp>
#include <mbgl/text/rotation_range.hpp>
#include <mbgl/util/math.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#pragma GCC diagnostic pop

namespace mbgl {
namespace bg = boost::geometry;
namespace bgm = bg::model;
namespace bgi = bg::index;
typedef bgm::point<float, 2, bg::cs::cartesian> Point;
typedef bgm::box<Point> Box;
typedef std::pair<Box, PlacementBox> PlacementValue;
typedef bgi::rtree<PlacementValue, bgi::rstar<16>> Tree;
}

using namespace mbgl;

Collision::~Collision() {
    delete ((Tree *)cTree);
    delete ((Tree *)hTree);
}

Collision::Collision(float zoom, float tileExtent, float tileSize, float placementDepth)
    : hTree(new Tree()), // tree for horizontal labels
      cTree(new Tree()), // tree for glyphs from curved labels

      // tile pixels per screen pixels at the tile's zoom level
      tilePixelRatio(tileExtent / tileSize),

      zoom(zoom),

      // Calculate the maximum scale we can go down in our fake-3d rtree so that
      // placement still makes sense. This is calculated so that the minimum
      // placement zoom can be at most 25.5 (we use an unsigned integer x10 to
      // store the minimum zoom).
      //
      // We don't want to place labels all the way to 25.5. This lets too many
      // glyphs be placed, slowing down collision checking. Only place labels if
      // they will show up within the intended zoom range of the tile.
      maxPlacementScale(std::exp(std::log(2) * util::min(3.0f, placementDepth, 25.5f - zoom)))
{
    const float m = 4096;
    const float edge = m * tilePixelRatio * 2;

    // Hack to prevent cross-tile labels
    insert(
        {{GlyphBox{
              CollisionRect{CollisionPoint{0, 0}, CollisionPoint{0, m * 8}}, 0},
          GlyphBox{
              CollisionRect{CollisionPoint{0, 0}, CollisionPoint{m * 8, 0}}, 0}}},
        CollisionAnchor(0, 0), 1, {{M_PI * 2, 0}}, false, 2);

    insert({{GlyphBox{
                 CollisionRect{CollisionPoint{-m * 8, 0}, CollisionPoint{0, 0}}, 0},
             GlyphBox{
                 CollisionRect{CollisionPoint{0, -m * 8}, CollisionPoint{0, 0}}, 0}}},
           CollisionAnchor{m, m}, 1, {{M_PI * 2, 0}}, false, 2);
}

GlyphBox getMergedGlyphs(const GlyphBoxes &boxes, const CollisionAnchor &anchor) {
    GlyphBox mergedGlyphs;
    const float inf = std::numeric_limits<float>::infinity();
    mergedGlyphs.box = CollisionRect{{inf, inf}, {-inf, -inf}};
    mergedGlyphs.anchor = anchor;

    CollisionRect &box = mergedGlyphs.box;
    for (const GlyphBox &glyph : boxes) {
        const CollisionRect &gbox = glyph.box;
        box.tl.x = util::min(box.tl.x, gbox.tl.x);
        box.tl.y = util::min(box.tl.y, gbox.tl.y);
        box.br.x = util::max(box.br.x, gbox.br.x);
        box.br.y = util::max(box.br.y, gbox.br.y);
        mergedGlyphs.minScale =
            util::max(mergedGlyphs.minScale, glyph.minScale);
    }

    return mergedGlyphs;
}

PlacementProperty Collision::place(const GlyphBoxes &boxes, const CollisionAnchor &anchor,
                                   float minPlacementScale, float maxPlacementScale, float padding,
                                   bool horizontal, bool allowOverlap, bool ignorePlacement) {
    float minScale = std::numeric_limits<float>::infinity();
    for (const GlyphBox &glyphBox : boxes) {
        minScale = util::min(minScale, glyphBox.minScale);
    }
    minPlacementScale = util::max(minPlacementScale, minScale);

    GlyphBoxes glyphs;
    if (horizontal) {
        // Collision checks between rotating and fixed labels are relatively expensive,
        // so we use one box per label, not per glyph for horizontal labels.
        glyphs.push_back(getMergedGlyphs(boxes, anchor));

        // for all horizontal labels, calculate bbox covering all rotated positions
        const CollisionRect &box = glyphs.front().box;
        float x12 = box.tl.x * box.tl.x,
              y12 = box.tl.y * box.tl.y,
              x22 = box.br.x * box.br.x,
              y22 = box.br.y * box.br.y,
              diag = std::sqrt(util::max(x12 + y12, x12 + y22, x22 + y12, x22 + y22));

        glyphs.front().hBox = CollisionRect{
            {-diag, -diag},
            {diag, diag}
        };
    }

    // Calculate the minimum scale the entire label can be shown without
    // collisions
        float scale =
            allowOverlap ? minPlacementScale : getPlacementScale(glyphs, minPlacementScale,
                                                                  maxPlacementScale, padding);

    // Return if the label can never be placed without collision
    if (scale < 0 || scale == std::numeric_limits<float>::infinity()) {
        return PlacementProperty{};
    }

    // Calculate the range it is safe to rotate all glyphs
    PlacementRange rotationRange = allowOverlap ? PlacementRange{{2 * M_PI, 0.0f}}
                                                : getPlacementRange(glyphs, scale, horizontal);

    if (!ignorePlacement) insert(glyphs, anchor, scale, rotationRange, horizontal, padding);

    float zoom = log(scale) / log(2);

    return PlacementProperty{zoom, rotationRange};
}

float Collision::getPlacementScale(const GlyphBoxes &glyphs,
                                   float minPlacementScale,
                                   float maxPlacementScale, float pad) {

    for (const GlyphBox &glyph : glyphs) {
        const CollisionRect &box = glyph.box;
        const CollisionRect &bbox = glyph.hBox ? glyph.hBox.get() : glyph.box;
        const CollisionAnchor &anchor = glyph.anchor;

        if (anchor.x < 0 || anchor.x > 4096 || anchor.y < 0 ||
            anchor.y > 4096) {
            return -1;
        }

        float minScale = std::fmax(minPlacementScale, glyph.minScale);
        float maxScale = glyph.maxScale;

        if (minScale >= maxScale) {
            continue;
        }

        // Compute the scaled bounding box of the unrotated glyph
        float minPlacedX = anchor.x + bbox.tl.x / minScale;
        float minPlacedY = anchor.y + bbox.tl.y / minScale;
        float maxPlacedX = anchor.x + bbox.br.x / minScale;
        float maxPlacedY = anchor.y + bbox.br.y / minScale;

        Box query_box{Point{minPlacedX, minPlacedY},
                      Point{maxPlacedX, maxPlacedY}};

        std::vector<PlacementValue> blocking;
        ((Tree *)cTree)
            ->query(bgi::intersects(query_box), std::back_inserter(blocking));
        ((Tree *)hTree)
            ->query(bgi::intersects(query_box), std::back_inserter(blocking));

        if (blocking.size()) {
            const CollisionAnchor &na = anchor; // new anchor
            const CollisionRect &nb = box;      // new box

            for (const PlacementValue &value : blocking) {
                const PlacementBox &placement = std::get<1>(value);
                const CollisionAnchor &oa = placement.anchor; // old anchor
                const CollisionRect &ob = placement.box;      // old box

                // If anchors are identical, we're going to skip the label.
                // NOTE: this isn't right because there can be glyphs with
                // the same anchor but differing box offsets.
                if (na == oa) {
                    return -1;
                }

                // todo: unhardcode the 8 = tileExtent/tileSize
                float padding = std::fmax(pad, placement.padding) * 8.0f;

                // Original algorithm:
                float s1 = (ob.tl.x - nb.br.x - padding) / (na.x - oa.x); // scale at which new box is to the left of old box
                float s2 = (ob.br.x - nb.tl.x + padding) / (na.x - oa.x); // scale at which new box is to the right of old box
                float s3 = (ob.tl.y - nb.br.y - padding) / (na.y - oa.y); // scale at which new box is to the top of old box
                float s4 = (ob.br.y - nb.tl.y + padding) / (na.y - oa.y); // scale at which new box is to the bottom of old box

                if (isnan(s1) || isnan(s2)) {
                    s1 = s2 = 1;
                }
                if (isnan(s3) || isnan(s4)) {
                    s3 = s4 = 1;
                }

                float collisionFreeScale = std::fmin(std::fmax(s1, s2), std::fmax(s3, s4));

                // Only update label's min scale if the glyph was
                // restricted by a collision
                if (collisionFreeScale > minPlacementScale &&
                    collisionFreeScale > minScale &&
                    collisionFreeScale < maxScale &&
                    collisionFreeScale < placement.maxScale) {
                    minPlacementScale = collisionFreeScale;
                }

                if (minPlacementScale > maxPlacementScale) {
                    return -1;
                }
            }
        }
    }

    return minPlacementScale;
}

PlacementRange Collision::getPlacementRange(const GlyphBoxes &glyphs,
                                            float placementScale,
                                            bool horizontal) {
    PlacementRange placementRange = {{2.0f * M_PI, 0}};

    for (const GlyphBox &glyph : glyphs) {
        const CollisionRect &bbox = glyph.hBox ? glyph.hBox.get() : glyph.box;
        const CollisionAnchor &anchor = glyph.anchor;

        float minPlacedX = anchor.x + bbox.tl.x / placementScale;
        float minPlacedY = anchor.y + bbox.tl.y / placementScale;
        float maxPlacedX = anchor.x + bbox.br.x / placementScale;
        float maxPlacedY = anchor.y + bbox.br.y / placementScale;

        Box query_box{Point{minPlacedX, minPlacedY},
                      Point{maxPlacedX, maxPlacedY}};

        std::vector<PlacementValue> blocking;
        ((Tree *)hTree)
            ->query(bgi::intersects(query_box), std::back_inserter(blocking));

        if (horizontal) {
            ((Tree *)cTree)
                ->query(bgi::intersects(query_box), std::back_inserter(blocking));
        }

        for (const PlacementValue &value : blocking) {
            const Box &s = std::get<0>(value);
            const PlacementBox &b = std::get<1>(value);
            const CollisionRect &bbox2 = b.hBox ? b.hBox.get() : b.box;

            float x1, x2, y1, y2, intersectX, intersectY;

            // Adjust and compare bboxes to see if the glyphs might intersect
            if (placementScale > b.placementScale) {
                x1 = b.anchor.x + bbox2.tl.x / placementScale;
                y1 = b.anchor.y + bbox2.tl.y / placementScale;
                x2 = b.anchor.x + bbox2.br.x / placementScale;
                y2 = b.anchor.y + bbox2.br.y / placementScale;
                intersectX = x1 < maxPlacedX && x2 > minPlacedX;
                intersectY = y1 < maxPlacedY && y2 > minPlacedY;
            } else {
                x1 = anchor.x + bbox.tl.x / b.placementScale;
                y1 = anchor.y + bbox.tl.y / b.placementScale;
                x2 = anchor.x + bbox.br.x / b.placementScale;
                y2 = anchor.y + bbox.br.y / b.placementScale;

                intersectX = x1 < s.max_corner().get<0>() &&
                             x2 > s.min_corner().get<0>();
                intersectY = y1 < s.max_corner().get<1>() &&
                             y2 > s.min_corner().get<1>();
            }

            // If they can't intersect, skip more expensive rotation calculation
            if (!(intersectX && intersectY))
                continue;

            float scale = std::fmax(placementScale, b.placementScale);
            // TODO? glyph.box or glyph.bbox?
            CollisionRange range = rotationRange(glyph, b, scale);

            placementRange[0] = std::fmin(placementRange[0], range[0]);
            placementRange[1] = std::fmax(placementRange[1], range[1]);
        }
    }

    return placementRange;
};

void Collision::insert(const GlyphBoxes &glyphs, const CollisionAnchor &anchor,
                       float placementScale,
                       const PlacementRange &placementRange, bool horizontal,
                       float padding) {
    assert(placementScale != std::numeric_limits<float>::infinity());

    std::vector<PlacementValue> result;
    result.reserve(glyphs.size());

    for (const GlyphBox &glyph : glyphs) {
        const CollisionRect &box = glyph.box;
        const CollisionRect &bbox = glyph.hBox ? glyph.hBox.get() : glyph.box;

        float minScale = std::fmax(placementScale, glyph.minScale);

        Box bounds{Point{anchor.x + bbox.tl.x / minScale,
                         anchor.y + bbox.tl.y / minScale},
                   Point{anchor.x + bbox.br.x / minScale,
                         anchor.y + bbox.br.y / minScale}};

        PlacementBox placement;
        placement.anchor = anchor;
        placement.box = box;
        if (glyph.hBox) {
            placement.hBox = bbox;
        }
        placement.placementRange = placementRange;
        placement.placementScale = minScale;
        placement.maxScale = glyph.maxScale;
        placement.padding = padding;

        assert(!isnan(bounds.min_corner().get<0>()) && !isnan(bounds.min_corner().get<1>()));
        assert(!isnan(bounds.max_corner().get<0>()) && !isnan(bounds.max_corner().get<1>()));

        result.emplace_back(bounds, placement);
    }

    // Bulk-insert all glyph boxes
    if (horizontal) {
        ((Tree *)hTree)->insert(result.begin(), result.end());
    } else {
        ((Tree *)cTree)->insert(result.begin(), result.end());
    }
}
