#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/annotation/symbol_annotation_impl.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <mutex>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace mbgl {

class LatLngBounds;
class AnnotationTile;
class AnnotationTileData;
class SymbolAnnotationImpl;
class ShapeAnnotationImpl;

class AnnotationManager : private util::noncopyable {
public:
    AnnotationManager();
    ~AnnotationManager();

    AnnotationID addAnnotation(const Annotation&, const uint8_t maxZoom);
    Update updateAnnotation(const AnnotationID&, const Annotation&, const uint8_t maxZoom);
    void removeAnnotation(const AnnotationID&);

    void addImage(std::unique_ptr<style::Image>);
    void removeImage(const std::string&);
    double getTopOffsetPixelsForImage(const std::string&);

    void updateStyle(style::Style::Impl&);
    void updateData();

    void addTile(AnnotationTile&);
    void removeTile(AnnotationTile&);

    static const std::string SourceID;
    static const std::string PointLayerID;

private:
    void add(const AnnotationID&, const SymbolAnnotation&, const uint8_t);
    void add(const AnnotationID&, const LineAnnotation&, const uint8_t);
    void add(const AnnotationID&, const FillAnnotation&, const uint8_t);

    Update update(const AnnotationID&, const SymbolAnnotation&, const uint8_t);
    Update update(const AnnotationID&, const LineAnnotation&, const uint8_t);
    Update update(const AnnotationID&, const FillAnnotation&, const uint8_t);

    void removeAndAdd(const AnnotationID&, const Annotation&, const uint8_t);

    void remove(const AnnotationID&);

    std::unique_ptr<AnnotationTileData> getTileData(const CanonicalTileID&);

    std::mutex mutex;

    AnnotationID nextID = 0;

    using SymbolAnnotationTree = boost::geometry::index::rtree<std::shared_ptr<const SymbolAnnotationImpl>, boost::geometry::index::rstar<16, 4>>;
    // Unlike std::unordered_map, std::map is guaranteed to sort by AnnotationID, ensuring that older annotations are below newer annotations.
    // <https://github.com/mapbox/mapbox-gl-native/issues/5691>
    using SymbolAnnotationMap = std::map<AnnotationID, std::shared_ptr<SymbolAnnotationImpl>>;
    using ShapeAnnotationMap = std::map<AnnotationID, std::unique_ptr<ShapeAnnotationImpl>>;
    using ImageMap = std::unordered_map<std::string, style::Image>;

    SymbolAnnotationTree symbolTree;
    SymbolAnnotationMap symbolAnnotations;
    ShapeAnnotationMap shapeAnnotations;
    ImageMap images;
    std::unordered_set<std::string> obsoleteShapeAnnotationLayers;
    std::unordered_set<std::string> obsoleteImages;
    std::unordered_set<AnnotationTile*> tiles;

    friend class AnnotationTile;
};

} // namespace mbgl
