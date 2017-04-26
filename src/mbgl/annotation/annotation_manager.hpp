#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/annotation/symbol_annotation_impl.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <string>
#include <vector>
#include <unordered_set>

namespace mbgl {

class LatLngBounds;
class AnnotationTile;
class AnnotationTileData;
class SymbolAnnotationImpl;
class ShapeAnnotationImpl;

namespace style {
class Style;
class Image;
} // namespace style

class AnnotationManager : private util::noncopyable {
public:
    AnnotationManager(float pixelRatio);
    ~AnnotationManager();

    AnnotationID addAnnotation(const Annotation&, const uint8_t maxZoom);
    Update updateAnnotation(const AnnotationID&, const Annotation&, const uint8_t maxZoom);
    void removeAnnotation(const AnnotationID&);

    void addImage(const std::string&, std::unique_ptr<style::Image>);
    void removeImage(const std::string&);
    double getTopOffsetPixelsForImage(const std::string&);
    SpriteAtlas& getSpriteAtlas() { return spriteAtlas; }

    void updateStyle(style::Style&);
    void updateData();

    void addTile(AnnotationTile&);
    void removeTile(AnnotationTile&);

    static const std::string SourceID;
    static const std::string PointLayerID;

private:
    void add(const AnnotationID&, const SymbolAnnotation&, const uint8_t);
    void add(const AnnotationID&, const LineAnnotation&, const uint8_t);
    void add(const AnnotationID&, const FillAnnotation&, const uint8_t);
    void add(const AnnotationID&, const StyleSourcedAnnotation&, const uint8_t);

    Update update(const AnnotationID&, const SymbolAnnotation&, const uint8_t);
    Update update(const AnnotationID&, const LineAnnotation&, const uint8_t);
    Update update(const AnnotationID&, const FillAnnotation&, const uint8_t);
    Update update(const AnnotationID&, const StyleSourcedAnnotation&, const uint8_t);

    void removeAndAdd(const AnnotationID&, const Annotation&, const uint8_t);

    std::unique_ptr<AnnotationTileData> getTileData(const CanonicalTileID&);

    AnnotationID nextID = 0;

    using SymbolAnnotationTree = boost::geometry::index::rtree<std::shared_ptr<const SymbolAnnotationImpl>, boost::geometry::index::rstar<16, 4>>;
    // Unlike std::unordered_map, std::map is guaranteed to sort by AnnotationID, ensuring that older annotations are below newer annotations.
    // <https://github.com/mapbox/mapbox-gl-native/issues/5691>
    using SymbolAnnotationMap = std::map<AnnotationID, std::shared_ptr<SymbolAnnotationImpl>>;
    using ShapeAnnotationMap = std::map<AnnotationID, std::unique_ptr<ShapeAnnotationImpl>>;

    SymbolAnnotationTree symbolTree;
    SymbolAnnotationMap symbolAnnotations;
    ShapeAnnotationMap shapeAnnotations;
    std::unordered_set<std::string> obsoleteShapeAnnotationLayers;
    std::unordered_set<AnnotationTile*> tiles;
    SpriteAtlas spriteAtlas;

    friend class AnnotationTile;
};

} // namespace mbgl
