#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/util/geo.hpp>

#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wshadow"
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wdeprecated-register"
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#pragma GCC diagnostic pop

// Make Boost Geometry aware of our LatLng type
BOOST_GEOMETRY_REGISTER_POINT_2D_CONST(mbgl::LatLng, double, boost::geometry::cs::cartesian, longitude(), latitude())
BOOST_GEOMETRY_REGISTER_BOX(mbgl::LatLngBounds, mbgl::LatLng, southwest(), northeast())

namespace mbgl {

class AnnotationTileLayer;
class CanonicalTileID;

class SymbolAnnotationImpl {
public:
    SymbolAnnotationImpl(AnnotationID, SymbolAnnotation);

    void updateLayer(const CanonicalTileID&, AnnotationTileLayer&) const;

    const AnnotationID id;
    const SymbolAnnotation annotation;
};

} // namespace mbgl

// Tell Boost Geometry how to access a std::shared_ptr<mbgl::SymbolAnnotation> object.
namespace boost {
namespace geometry {
namespace index {

template <>
struct indexable<std::shared_ptr<const mbgl::SymbolAnnotationImpl>> {
    using result_type = mbgl::LatLng;
    mbgl::LatLng operator()(const std::shared_ptr<const mbgl::SymbolAnnotationImpl>& v) const {
        const mbgl::Point<double>& p = v->annotation.geometry;
        return mbgl::LatLng(p.y, p.x);
    }
};

} // end namespace index
} // end namespace geometry
} // end namespace boost
