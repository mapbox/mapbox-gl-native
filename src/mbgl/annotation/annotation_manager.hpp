#ifndef MBGL_ANNOTATION_MANAGER
#define MBGL_ANNOTATION_MANAGER

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/annotation/point_annotation_impl.hpp>
#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <string>
#include <vector>

namespace mbgl {

class PointAnnotation;
class ShapeAnnotation;
class AnnotationTile;
class Style;

class AnnotationManager : private util::noncopyable {
public:
    AnnotationManager();
    ~AnnotationManager();

    AnnotationIDs addPointAnnotations(const std::vector<PointAnnotation>&, const uint8_t maxZoom);
    AnnotationIDs addShapeAnnotations(const std::vector<ShapeAnnotation>&, const uint8_t maxZoom);
    void removeAnnotations(const AnnotationIDs&);

    AnnotationIDs getPointAnnotationsInBounds(const LatLngBounds&) const;
    LatLngBounds getBoundsForAnnotations(const AnnotationIDs&) const;

    void updateStyle(Style&);
    std::unique_ptr<AnnotationTile> getTile(const TileID&);

    static const std::string SourceID;
    static const std::string PointLayerID;

private:
    AnnotationID nextID = 0;
    PointAnnotationImpl::Tree pointTree;
    PointAnnotationImpl::Map pointAnnotations;
    ShapeAnnotationImpl::Map shapeAnnotations;
    std::vector<std::string> obsoleteShapeAnnotationLayers;
};

}

#endif
