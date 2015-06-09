#include <mbgl/annotation/point_annotation_layer.hpp>

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
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#pragma GCC diagnostic pop

namespace bg = boost::geometry;

// Make Boost Geometry aware of our LatLng type
BOOST_GEOMETRY_REGISTER_POINT_2D(mbgl::LatLng, double, bg::cs::cartesian, longitude, latitude)
BOOST_GEOMETRY_REGISTER_BOX(mbgl::LatLngBounds, mbgl::LatLng, sw, ne)

// Tell Boost Geometry how to access a std::shared_ptr<mbgl::PointAnnotation> object.
namespace boost {
namespace geometry {
namespace index {

template <>
struct indexable<std::shared_ptr<const mbgl::Annotation>> {
    using result_type = const mbgl::LatLng&;
    inline const mbgl::LatLng& operator()(const std::shared_ptr<const mbgl::Annotation>& v) const {
        return v->point;
    }
};

} // end namespace index
} // end namespace geometry
} // end namespace boost

namespace mbgl {

class PointAnnotationLayer::Tree {
public:
    using Value = std::shared_ptr<const Annotation>;
    bg::index::rtree<Value, bg::index::rstar<16, 4>> data;
};

PointAnnotationLayer::PointAnnotationLayer() : tree(std::make_unique<Tree>()) {
}

PointAnnotationLayer::~PointAnnotationLayer() = default;

void PointAnnotationLayer::add(const std::shared_ptr<const Annotation>& point) {
    tree->data.insert(point);
}

void PointAnnotationLayer::add(const std::vector<std::shared_ptr<const Annotation>>& points) {
    tree->data.insert(points);
}

void PointAnnotationLayer::remove(const std::shared_ptr<const Annotation>& point) {
    tree->data.remove(point);
}

void PointAnnotationLayer::remove(const std::vector<std::shared_ptr<const Annotation>>& points) {
    tree->data.remove(points);
}

std::vector<std::shared_ptr<const Annotation>> PointAnnotationLayer::getInBounds(const LatLngBounds& bounds) const {
    std::vector<std::shared_ptr<const Annotation>> result;
    tree->data.query(bg::index::intersects(bounds), std::back_inserter(result));
    return result;
}

} // end namespace mbgl
