#ifndef MBGL_ANNOTATION_MANAGER
#define MBGL_ANNOTATION_MANAGER

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/annotation/point_annotation_impl.hpp>
#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/sprite/sprite_store.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <string>
#include <vector>
#include <set>

namespace mbgl {

class PointAnnotation;
class ShapeAnnotation;
class AnnotationTile;
class AnnotationTileMonitor;
class Style;

class AnnotationManager : private util::noncopyable {
public:
    AnnotationManager(float pixelRatio);
    ~AnnotationManager();

    AnnotationIDs addPointAnnotations(const std::vector<PointAnnotation>&, const uint8_t maxZoom);
    AnnotationIDs addShapeAnnotations(const std::vector<ShapeAnnotation>&, const uint8_t maxZoom);
    void updatePointAnnotation(const AnnotationID&, const PointAnnotation&, const uint8_t maxZoom);
    void removeAnnotations(const AnnotationIDs&);

    AnnotationIDs getPointAnnotationsInBounds(const LatLngBounds&) const;

    void addIcon(const std::string& name, std::shared_ptr<const SpriteImage>);
    void removeIcon(const std::string& name);
    double getTopOffsetPixelsForIcon(const std::string& name);
    SpriteAtlas& getSpriteAtlas() { return spriteAtlas; }

    void updateStyle(Style&);

    void addTileMonitor(AnnotationTileMonitor&);
    void removeTileMonitor(AnnotationTileMonitor&);

    static const std::string SourceID;
    static const std::string PointLayerID;

private:
    std::unique_ptr<AnnotationTile> getTile(const CanonicalTileID&);

    AnnotationID nextID = 0;
    PointAnnotationImpl::Tree pointTree;
    PointAnnotationImpl::Map pointAnnotations;
    ShapeAnnotationImpl::Map shapeAnnotations;
    std::vector<std::string> obsoleteShapeAnnotationLayers;
    std::set<AnnotationTileMonitor*> monitors;

    SpriteStore spriteStore;
    SpriteAtlas spriteAtlas;
};

} // namespace mbgl

#endif
