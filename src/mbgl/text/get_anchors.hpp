#pragma once

#include <mbgl/geometry/anchor.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

Anchors getAnchors(const GeometryCoordinates& line,
                   float spacing,
                   const float maxAngle,
                   const float textLeft,
                   const float textRight,
                   const float iconLeft,
                   const float iconRight,
                   const float glyphSize,
                   const float boxScale,
                   const float overscaling);

optional<Anchor> getCenterAnchor(const GeometryCoordinates& line,
                                 const float maxAngle,
                                 const float textLeft,
                                 const float textRight,
                                 const float iconLeft,
                                 const float iconRight,
                                 const float glyphSize,
                                 const float boxScale);
    

} // namespace mbgl
