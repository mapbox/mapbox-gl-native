#pragma once

#include <mapbox/geojsonvt.hpp>

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/annotation/annotation_impl.hpp>
#include <mbgl/util/geo.hpp>

#include <string>
#include <memory>

namespace mbgl {

class AnnotationTileData;
class CanonicalTileID;

namespace style {
class Style;
} // namespace style

class ShapeAnnotationImpl {
public:
    ShapeAnnotationImpl(const AnnotationID, const uint8_t maxZoom);
    virtual ~ShapeAnnotationImpl() = default;

    virtual void updateStyle(style::Style&) const = 0;
    virtual const ShapeAnnotationGeometry& geometry() const = 0;

    void updateTileData(const CanonicalTileID&, AnnotationTileData&);

    const AnnotationID id;
    const uint8_t maxZoom;
    const std::string layerID;
    std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> shapeTiler;
};

} // namespace mbgl

// Tell Boost Geometry how to access a std::shared_ptr<mbgl::ShapeAnnotationImpl> object.
namespace boost {
namespace geometry {
namespace index {

template <>
struct indexable<std::shared_ptr<const mbgl::ShapeAnnotationImpl>> {
    using result_type = mbgl::LatLngBounds;
    mbgl::LatLngBounds operator()(const std::shared_ptr<const mbgl::ShapeAnnotationImpl>& v) const {
        return mbgl::ShapeAnnotationGeometry::visit(v->geometry(), [&] (const auto& geometry) {
            return mbgl::boundsForGeometry(geometry);
        });
    }
};

} // end namespace index
} // end namespace geometry
} // end namespace boost
