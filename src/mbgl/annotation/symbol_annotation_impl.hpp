#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/util/geo.hpp>

#include <string>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/box.hpp>
#include <boost/geometry/index/rtree.hpp>

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

namespace boost {
namespace geometry {

// Make Boost Geometry aware of our LatLng type
namespace traits {

template<> struct tag<mbgl::LatLng> { using type = point_tag; };
template<> struct dimension<mbgl::LatLng> : boost::mpl::int_<2> {};
template<> struct coordinate_type<mbgl::LatLng> { using type = double; };
template<> struct coordinate_system<mbgl::LatLng> { using type = boost::geometry::cs::cartesian; };

template<> struct access<mbgl::LatLng, 0> { static inline double get(mbgl::LatLng const& p) { return p.longitude(); } };
template<> struct access<mbgl::LatLng, 1> { static inline double get(mbgl::LatLng const& p) { return p.latitude(); } };

template<> struct tag<mbgl::LatLngBounds> { using type = box_tag; };
template<> struct point_type<mbgl::LatLngBounds> { using type = mbgl::LatLng; };

template <size_t D>
struct indexed_access<mbgl::LatLngBounds, min_corner, D>
{
    using ct = coordinate_type<mbgl::LatLng>::type;
    static inline ct get(mbgl::LatLngBounds const& b) { return geometry::get<D>(b.southwest()); }
    static inline void set(mbgl::LatLngBounds& b, ct const& value) { geometry::set<D>(b.southwest(), value); }
};

template <size_t D>
struct indexed_access<mbgl::LatLngBounds, max_corner, D>
{
    using ct = coordinate_type<mbgl::LatLng>::type;
    static inline ct get(mbgl::LatLngBounds const& b) { return geometry::get<D>(b.northeast());  }
    static inline void set(mbgl::LatLngBounds& b, ct const& value) { geometry::set<D>(b.northeast(), value); }
};

} // namespace traits

// Tell Boost Geometry how to access a std::shared_ptr<mbgl::SymbolAnnotation> object.
namespace index {

template <>
struct indexable<std::shared_ptr<const mbgl::SymbolAnnotationImpl>> {
    using result_type = mbgl::LatLng;
    mbgl::LatLng operator()(const std::shared_ptr<const mbgl::SymbolAnnotationImpl>& v) const {
        const mbgl::Point<double>& p = v->annotation.geometry;
        return mbgl::LatLng(p.y, p.x);
    }
};

} // namespace index

} // namespace geometry
} // namespace boost
