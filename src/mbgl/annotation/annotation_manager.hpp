#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/annotation/symbol_annotation_impl.hpp>
#include <mbgl/sprite/sprite_store.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <string>
#include <vector>
#include <set>
#include <unordered_map>

namespace mbgl {

class AnnotationTile;
class AnnotationTileData;
class SymbolAnnotationImpl;
class ShapeAnnotationImpl;

namespace style {
class Style;
}

class AnnotationManager : private util::noncopyable {
public:
    AnnotationManager(float pixelRatio);
    ~AnnotationManager();

    AnnotationID addAnnotation(const Annotation&, const uint8_t maxZoom);
    void updateAnnotation(const AnnotationID&, const Annotation&, const uint8_t maxZoom);
    void removeAnnotation(const AnnotationID&);

    AnnotationIDs getPointAnnotationsInBounds(const LatLngBounds&) const;

    void addIcon(const std::string& name, std::shared_ptr<const SpriteImage>);
    void removeIcon(const std::string& name);
    double getTopOffsetPixelsForIcon(const std::string& name);
    SpriteAtlas& getSpriteAtlas() { return spriteAtlas; }

    void updateStyle(style::Style&);

    void addTile(AnnotationTile&);
    void removeTile(AnnotationTile&);

    static const std::string SourceID;
    static const std::string PointLayerID;

private:
    void add(const AnnotationID&, const SymbolAnnotation&, const uint8_t);
    void add(const AnnotationID&, const LineAnnotation&, const uint8_t);
    void add(const AnnotationID&, const FillAnnotation&, const uint8_t);
    void add(const AnnotationID&, const StyleSourcedAnnotation&, const uint8_t);

    std::unique_ptr<AnnotationTileData> getTileData(const CanonicalTileID&);

    AnnotationID nextID = 0;

    using SymbolAnnotationTree = boost::geometry::index::rtree<std::shared_ptr<const SymbolAnnotationImpl>, boost::geometry::index::rstar<16, 4>>;
    using SymbolAnnotationMap = std::unordered_map<AnnotationID, std::shared_ptr<SymbolAnnotationImpl>>;
    using ShapeAnnotationMap = std::unordered_map<AnnotationID, std::unique_ptr<ShapeAnnotationImpl>>;

    SymbolAnnotationTree symbolTree;
    SymbolAnnotationMap symbolAnnotations;
    ShapeAnnotationMap shapeAnnotations;
    std::vector<std::string> obsoleteShapeAnnotationLayers;
    std::set<AnnotationTile*> tiles;

    SpriteStore spriteStore;
    SpriteAtlas spriteAtlas;
};

} // namespace mbgl
