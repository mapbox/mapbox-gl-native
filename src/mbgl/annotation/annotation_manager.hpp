#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/annotation/symbol_annotation_feature.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>

namespace mbgl {

class ShapeAnnotationImpl;

namespace style {
class Style;
} // namespace style

/**
 * The AnnotationManager keeps track of Annotations separately from styles
 * so that the annotations persist over style changes.
 *
 * Only used from the main thread.
 */
class AnnotationManager : private util::noncopyable {
public:
    AnnotationManager();
    ~AnnotationManager();

    AnnotationID addAnnotation(const Annotation&, const uint8_t maxZoom);
    void updateAnnotation(const AnnotationID&, const Annotation&, const uint8_t maxZoom);
    void removeAnnotation(const AnnotationID&);

    void addImage(std::unique_ptr<style::Image>);
    void removeImage(const std::string&);
    double getTopOffsetPixelsForImage(const std::string&);

    static const std::string SourceID;
    static const std::string PointLayerID;

    void onStyleLoaded(style::Style& style);

private:
    void add(const AnnotationID&, const SymbolAnnotation&, const uint8_t);
    void add(const AnnotationID&, const LineAnnotation&, const uint8_t);
    void add(const AnnotationID&, const FillAnnotation&, const uint8_t);

    void update(const AnnotationID&, const SymbolAnnotation&, const uint8_t);
    void update(const AnnotationID&, const LineAnnotation&, const uint8_t);
    void update(const AnnotationID&, const FillAnnotation&, const uint8_t);

    // The current style, if any
    style::Style* style = nullptr;

    // The latest source Impl (for re-creating the source
    // on style changes)
    optional<Immutable<AnnotationSource::Impl>> sourceImpl;

    // Before any style is loaded, we need a source to
    // store our data
    std::unique_ptr<AnnotationSource> ownedSource;
    AnnotationSource* source = nullptr;

    AnnotationID nextID = 0;

    // Keep track of the shape annotations to be able to
    // recreate changes to newly loaded styles
    using ShapeAnnotationLayerMap = std::map<AnnotationID, std::unique_ptr<ShapeAnnotationImpl>>;
    ShapeAnnotationLayerMap shapeAnnotations;

    // Keep track of Images so that we can re-add
    // to newly loaded styles
    using ImageMap = std::unordered_map<std::string, style::Image>;
    ImageMap images;
};

} // namespace mbgl
