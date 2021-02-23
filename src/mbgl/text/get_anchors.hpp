#pragma once

#include <mbgl/geometry/anchor.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

Anchors getAnchors(const GeometryCoordinates& line,
                   float spacing,
                   float maxAngle,
                   float textLeft,
                   float textRight,
                   float iconLeft,
                   float iconRight,
                   float glyphSize,
                   float boxScale,
                   float overscaling);

optional<Anchor> getCenterAnchor(const GeometryCoordinates& line,
                                 float maxAngle,
                                 float textLeft,
                                 float textRight,
                                 float iconLeft,
                                 float iconRight,
                                 float glyphSize,
                                 float boxScale);

} // namespace mbgl
