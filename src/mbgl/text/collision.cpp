#include <mbgl/text/collision.hpp>
#include <mbgl/text/rotation_range.hpp>
#include <mbgl/util/math.hpp>


using namespace mbgl;

Box getBox(const CollisionAnchor &anchor, const CollisionRect &bbox, float minScale,
           float maxScale) {
    return Box{
        Point{
            anchor.x + util::min(bbox.tl.x / minScale, bbox.tl.x / maxScale),
            anchor.y + util::min(bbox.tl.y / minScale, bbox.tl.y / maxScale),
        },
        Point{
            anchor.x + util::max(bbox.br.x / minScale, bbox.br.x / maxScale),
            anchor.y + util::max(bbox.br.y / minScale, bbox.br.y / maxScale),
        },
    };
};

Collision::Collision(float zoom_, float tileExtent, float tileSize, float placementDepth)
      // tile pixels per screen pixels at the tile's zoom level
    : tilePixelRatio(tileExtent / tileSize),

      zoom(zoom_),

      // Calculate the maximum scale we can go down in our fake-3d rtree so that
      // placement still makes sense. This is calculated so that the minimum
      // placement zoom can be at most 25.5 (we use an unsigned integer x10 to
      // store the minimum zoom).
      //
      // We don't want to place labels all the way to 25.5. This lets too many
      // glyphs be placed, slowing down collision checking. Only place labels if
      // they will show up within the intended zoom range of the tile.
      maxPlacementScale(std::exp(std::log(2) * util::min(3.0f, placementDepth, 25.5f - zoom_))) {
    const float m = 4096;
    const float edge = m * tilePixelRatio * 2;


    PlacementBox left;
    left.anchor = CollisionAnchor{ 0.0f, 0.0f };
    left.box = CollisionRect{CollisionPoint{-edge, -edge}, CollisionPoint{0, edge}};
    left.placementRange = {{ 2.0f * M_PI, 0.0f }};
    left.placementScale = 0.5f;
    left.maxScale = std::numeric_limits<float>::infinity();
    left.padding = 0.0f;
    leftEdge = PlacementValue{
        getBox(left.anchor, left.box, left.placementScale, left.maxScale),
        left};

    PlacementBox top;
    top.anchor = CollisionAnchor{ 0.0f, 0.0f };
    top.box = CollisionRect{CollisionPoint{-edge, -edge}, CollisionPoint{edge, 0}};
    top.placementRange = {{ 2.0f * M_PI, 0.0f }};
    top.placementScale = 0.5f;
    top.maxScale = std::numeric_limits<float>::infinity();
    top.padding = 0.0f;
    topEdge = PlacementValue{
        getBox(top.anchor, top.box, top.placementScale, top.maxScale),
        top};

    PlacementBox bottom;
    bottom.anchor = CollisionAnchor{ m, m };
    bottom.box = CollisionRect{CollisionPoint{-edge, 0}, CollisionPoint{edge, edge}};
    bottom.placementRange = {{ 2.0f * M_PI, 0.0f }};
    bottom.placementScale = 0.5f;
    bottom.maxScale = std::numeric_limits<float>::infinity();
    bottom.padding = 0.0f;
    bottomEdge = PlacementValue{
        getBox(bottom.anchor, bottom.box, bottom.placementScale, bottom.maxScale),
        bottom};

    PlacementBox right;
    right.anchor = CollisionAnchor{ m, m };
    right.box = CollisionRect{CollisionPoint{0, -edge}, CollisionPoint{edge, edge}};
    right.placementRange = {{ 2.0f * M_PI, 0.0f }};
    right.placementScale = 0.5f;
    right.maxScale = std::numeric_limits<float>::infinity();
    right.padding = 0.0f;
    rightEdge = PlacementValue{
        getBox(right.anchor, right.box, right.placementScale, right.maxScale),
        right};

}

GlyphBox getMergedGlyphs(const GlyphBoxes &boxes, const CollisionAnchor &anchor) {
    GlyphBox mergedGlyphs;
    const float inf = std::numeric_limits<float>::infinity();
    mergedGlyphs.box = CollisionRect{{inf, inf}, {-inf, -inf}};
    mergedGlyphs.anchor = anchor;

    CollisionRect &box = mergedGlyphs.box;
    for (const auto& glyph : boxes) {
        const CollisionRect &gbox = glyph.box;
        box.tl.x = util::min(box.tl.x, gbox.tl.x);
        box.tl.y = util::min(box.tl.y, gbox.tl.y);
        box.br.x = util::max(box.br.x, gbox.br.x);
        box.br.y = util::max(box.br.y, gbox.br.y);
        mergedGlyphs.minScale = util::max(mergedGlyphs.minScale, glyph.minScale);
    }

    return mergedGlyphs;
}

float Collision::getPlacementScale(const GlyphBoxes &glyphs, float minPlacementScale, bool avoidEdges) {

    for (const auto& glyph : glyphs) {
        const CollisionRect &box = glyph.box;
        const CollisionRect &bbox = glyph.hBox ? glyph.hBox.get() : glyph.box;
        const CollisionAnchor &anchor = glyph.anchor;
        const float pad = glyph.padding;


        if (anchor.x < 0 || anchor.x > 4096 || anchor.y < 0 || anchor.y > 4096) {
            return 0;
        }

        float minScale = std::fmax(minPlacementScale, glyph.minScale);
        float maxScale = glyph.maxScale != 0 ? glyph.maxScale : std::numeric_limits<float>::infinity();

        if (minScale >= maxScale) {
            continue;
        }

        // Compute the scaled bounding box of the unrotated glyph
        const Box searchBox = getBox(anchor, bbox, minScale, maxScale);

        std::vector<PlacementValue> blocking;
        hTree.query(bgi::intersects(searchBox), std::back_inserter(blocking));
        cTree.query(bgi::intersects(searchBox), std::back_inserter(blocking));

        if (avoidEdges) {
            if (searchBox.min_corner().get<0>() < 0) blocking.emplace_back(leftEdge);
            if (searchBox.min_corner().get<1>() < 0) blocking.emplace_back(topEdge);
            if (searchBox.max_corner().get<0>() >= 4096) blocking.emplace_back(rightEdge);
            if (searchBox.max_corner().get<1>() >= 4096) blocking.emplace_back(bottomEdge);
        }

        if (blocking.size()) {
            const CollisionAnchor &na = anchor; // new anchor
            const CollisionRect &nb = box;      // new box

            for (const auto& value : blocking) {
                const PlacementBox &placement = std::get<1>(value);
                const CollisionAnchor &oa = placement.anchor; // old anchor
                const CollisionRect &ob = placement.box;      // old box

                // If anchors are identical, we're going to skip the label.
                // NOTE: this isn't right because there can be glyphs with
                // the same anchor but differing box offsets.
                if (na == oa) {
                    return 0;
                }

                // todo: unhardcode the 8 = tileExtent/tileSize
                float padding = std::fmax(pad, placement.padding) * 8.0f;

                // Original algorithm:
                float sx = (na.x - oa.x);
                float s1 = 1;
                float s2 = 1;
                if (sx != 0) {
                   s1 = (ob.tl.x - nb.br.x - padding) /
                           sx; // scale at which new box is to the left of old box
                   s2 = (ob.br.x - nb.tl.x + padding) /
                           sx; // scale at which new box is to the right of old box
                }
                float sy = (na.y - oa.y);
                float s3 = 1;
                float s4 = 1;
                if (sy != 0) {
                    s3 = (ob.tl.y - nb.br.y - padding) /
                           sy; // scale at which new box is to the top of old box
                    s4 = (ob.br.y - nb.tl.y + padding) /
                           sy; // scale at which new box is to the bottom of old box
                }

                if (std::isnan(s1) || std::isnan(s2)) {
                    s1 = s2 = 1;
                }
                if (std::isnan(s3) || std::isnan(s4)) {
                    s3 = s4 = 1;
                }

                const float collisionFreeScale = std::fmin(std::fmax(s1, s2), std::fmax(s3, s4));

                // Only update label's min scale if the glyph was
                // restricted by a collision
                if (collisionFreeScale > minPlacementScale &&
                    collisionFreeScale > minScale &&
                    collisionFreeScale < maxScale &&
                    collisionFreeScale < placement.maxScale) {
                    minPlacementScale = collisionFreeScale;
                }

                if (minPlacementScale > maxPlacementScale) {
                    return 0;
                }
            }
        }
    }

    return minPlacementScale;
}

PlacementRange Collision::getPlacementRange(const GlyphBoxes &glyphs, float placementScale,
                                            bool horizontal) {
    PlacementRange placementRange = {{2.0f * M_PI, 0}};

    for (const auto& glyph : glyphs) {
        const CollisionRect &bbox = glyph.hBox ? glyph.hBox.get() : glyph.box;
        const CollisionAnchor &anchor = glyph.anchor;

        float minPlacedX = anchor.x + bbox.tl.x / placementScale;
        float minPlacedY = anchor.y + bbox.tl.y / placementScale;
        float maxPlacedX = anchor.x + bbox.br.x / placementScale;
        float maxPlacedY = anchor.y + bbox.br.y / placementScale;

        Box query_box{Point{minPlacedX, minPlacedY}, Point{maxPlacedX, maxPlacedY}};

        std::vector<PlacementValue> blocking;
        hTree.query(bgi::intersects(query_box), std::back_inserter(blocking));

        if (horizontal) {
            cTree.query(bgi::intersects(query_box), std::back_inserter(blocking));
        }

        for (const auto& value : blocking) {
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

                intersectX = x1 < s.max_corner().get<0>() && x2 > s.min_corner().get<0>();
                intersectY = y1 < s.max_corner().get<1>() && y2 > s.min_corner().get<1>();
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
                       float placementScale, const PlacementRange &placementRange,
                       bool horizontal) {
    assert(placementScale != std::numeric_limits<float>::infinity());

    std::vector<PlacementValue> allBounds;
    allBounds.reserve(glyphs.size());

    for (const auto& glyph : glyphs) {
        const CollisionRect &box = glyph.box;
        const CollisionRect &bbox = glyph.hBox ? glyph.hBox.get() : glyph.box;

        const float minScale = util::max(placementScale, glyph.minScale);
        const float maxScale = glyph.maxScale != 0 ? glyph.maxScale : std::numeric_limits<float>::infinity();

        const Box bounds = getBox(anchor, bbox, minScale, maxScale);

        PlacementBox placement;
        placement.anchor = anchor;
        placement.box = box;
        if (glyph.hBox) {
            placement.hBox = bbox;
        }
        placement.placementRange = placementRange;
        placement.placementScale = minScale;
        placement.maxScale = maxScale;
        placement.padding = glyph.padding;

        allBounds.emplace_back(bounds, placement);
    }

    // Bulk-insert all glyph boxes
    if (horizontal) {
        hTree.insert(allBounds.begin(), allBounds.end());
    } else {
        cTree.insert(allBounds.begin(), allBounds.end());
    }
}
