#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/style/source.hpp>

#include <memory>

namespace mbgl {

class ShapeAnnotationFeature;
class SymbolAnnotationFeature;

class AnnotationSource : public style::Source {
public:
    class Impl;

    AnnotationSource();
    AnnotationSource(Immutable<Impl>);

    Immutable<Impl> addAnnotation(const AnnotationID&, ShapeAnnotationGeometry, const uint8_t maxZoom);
    Immutable<Impl> addAnnotation(const AnnotationID&, const SymbolAnnotation&);

    Immutable<Impl> updateAnnotation(const AnnotationID&,
                                     ShapeAnnotationGeometry,
                                     const uint8_t);
    Immutable<Impl> updateAnnotation(const AnnotationID&, const SymbolAnnotation&);

    Immutable<Impl> removeAnnotation(const AnnotationID&);

    const Impl& impl() const;

    void loadDescription(FileSource&) final;

};

namespace style {

template<>
inline bool Source::is<AnnotationSource>() const {
    return getType() == SourceType::Annotations;
}

} // namespace style
} // namespace mbgl
