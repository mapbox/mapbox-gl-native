#pragma once

#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/annotation/shape_annotation_feature.hpp>
#include <mbgl/annotation/symbol_annotation_feature.hpp>
#include <mbgl/style/source_impl.hpp>
#include <mbgl/util/optional.hpp>

#include <memory>

namespace mbgl {

class AnnotationSource;
class AnnotationTileData;
class AnnotationTileLayer;
class CanonicalTileID;
//class ShapeAnnotationFeature;
//class SymbolAnnotationFeature;

class AnnotationData {
public:
    AnnotationData() = default;
    AnnotationData(const AnnotationData&) = default;
    ~AnnotationData() = default;

    std::unique_ptr<AnnotationTileData> getTile(const CanonicalTileID&) const;

private:
    using SymbolAnnotationTree = boost::geometry::index::rtree<std::shared_ptr<const SymbolAnnotationFeature>, boost::geometry::index::rstar<16, 4>>;
    // Unlike std::unordered_map, std::map is guaranteed to sort by AnnotationID, ensuring that older annotations are below newer annotations.
    // <https://github.com/mapbox/mapbox-gl-native/issues/5691>
    using SymbolAnnotationMap = std::map<const AnnotationID, std::shared_ptr<const SymbolAnnotationFeature>>;
    using ShapeAnnotationMap = std::map<const AnnotationID, std::shared_ptr<const ShapeAnnotationFeature>>;

    SymbolAnnotationTree symbolTree;
    SymbolAnnotationMap symbolAnnotations;
    ShapeAnnotationMap shapeAnnotations;

    friend class AnnotationSource;
};

class AnnotationSource::Impl : public style::Source::Impl {
public:
    Impl();
    Impl(const Impl&) = default;

    AnnotationData* getData();
    const AnnotationData* getData() const;

    optional<std::string> getAttribution() const final;

private:
    AnnotationData data;

};

} // namespace mbgl
