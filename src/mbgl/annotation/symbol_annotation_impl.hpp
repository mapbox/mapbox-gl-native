#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/annotation/annotation_impl.hpp>
#include <mbgl/util/geo.hpp>

#include <string>

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
