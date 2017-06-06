#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {

ShapeAnnotationImpl::ShapeAnnotationImpl(const AnnotationID id_, const uint8_t maxZoom_)
        : id(id_)
        , maxZoom(maxZoom_)
        , layerID("com.mapbox.annotations.shape." + util::toString(id)){
};


} // namespace mbgl
