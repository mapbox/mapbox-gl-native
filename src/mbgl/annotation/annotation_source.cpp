#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/annotation/symbol_annotation_feature.hpp>
#include <mbgl/annotation/shape_annotation_feature.hpp>
#include <mbgl/annotation/annotation_source_impl.hpp>
#include <mbgl/style/source_observer.hpp>

namespace mbgl {

using namespace style;

AnnotationSource::AnnotationSource()
        : Source(makeMutable<Impl>()) {
}

AnnotationSource::AnnotationSource(Immutable <Impl> impl)
        : Source(std::move(impl)) {
}

Immutable<AnnotationSource::Impl> AnnotationSource::addAnnotation(const AnnotationID &id,
                                                                  ShapeAnnotationGeometry geometry,
                                                                  const uint8_t maxZoom) {
    Mutable <Impl> mutableImpl = makeMutable<Impl>(impl());
    mutableImpl->getData()->shapeAnnotations.emplace(
            id,
            std::make_shared<ShapeAnnotationFeature>(id, std::move(geometry), maxZoom));
    baseImpl = Immutable<Source::Impl>(std::move(mutableImpl));

    observer->onSourceChanged(*this);

    return makeMutable<Impl>(impl());
};

Immutable<AnnotationSource::Impl> AnnotationSource::addAnnotation(const AnnotationID &id, const SymbolAnnotation &annotation) {
    auto feature = std::make_shared<SymbolAnnotationFeature>(id, annotation);

    Mutable <Impl> mutableImpl = makeMutable<Impl>(impl());
    mutableImpl->getData()->symbolTree.insert(feature);
    mutableImpl->getData()->symbolAnnotations.emplace(id, feature);
    baseImpl = Immutable<Source::Impl>(std::move(mutableImpl));

    observer->onSourceChanged(*this);

    return makeMutable<Impl>(impl());
};

Immutable<AnnotationSource::Impl> AnnotationSource::updateAnnotation(const AnnotationID &id,
                                                                     ShapeAnnotationGeometry geometry,
                                                                     const uint8_t maxZoom) {
    Mutable <Impl> mutableImpl = makeMutable<Impl>(impl());
    mutableImpl->getData()->shapeAnnotations.erase(id);
    mutableImpl->getData()->shapeAnnotations.emplace(
            id,
            std::make_shared<ShapeAnnotationFeature>(id, std::move(geometry), maxZoom));
    baseImpl = Immutable<Source::Impl>(std::move(mutableImpl));

    observer->onSourceChanged(*this);

    return makeMutable<Impl>(impl());
};

Immutable<AnnotationSource::Impl> AnnotationSource::updateAnnotation(const AnnotationID &id, const SymbolAnnotation &annotation) {
    auto it = impl().getData()->symbolAnnotations.find(id);
    if (it == impl().getData()->symbolAnnotations.end()) {
        // Attempt to update a non-existent symbol annotation
        assert(false);
    } else if (it->second->annotation.geometry != annotation.geometry ||
               it->second->annotation.icon != annotation.icon) {
        Mutable<Impl> mutableImpl = makeMutable<Impl>(impl());
        auto data = mutableImpl->getData();

        // Remove / add, update
        data->symbolTree.remove(it->second);
        auto feature = std::make_shared<SymbolAnnotationFeature>(id, annotation);
        data->symbolTree.insert(feature);
        data->symbolAnnotations[id] = std::move(feature);

        baseImpl = Immutable<Source::Impl>(std::move(mutableImpl));
        observer->onSourceChanged(*this);
    }

    return makeMutable<Impl>(impl());
};

Immutable<AnnotationSource::Impl> AnnotationSource::removeAnnotation(const AnnotationID& id) {
    Mutable <Impl> mutableImpl = makeMutable<Impl>(impl());
    auto data = mutableImpl->getData();

    if (data->symbolAnnotations.find(id) != data->symbolAnnotations.end()) {
        data->symbolTree.remove(data->symbolAnnotations.at(id));
        data->symbolAnnotations.erase(id);
    } else if (data->shapeAnnotations.find(id) !=data->shapeAnnotations.end()) {
        data->shapeAnnotations.erase(id);
    } else {
        assert(false); // Should never happen
    }

    baseImpl = Immutable<Source::Impl>(std::move(mutableImpl));

    observer->onSourceChanged(*this);

    return makeMutable<Impl>(impl());
};

const AnnotationSource::Impl &AnnotationSource::impl() const {
    return static_cast<const Impl &>(*baseImpl);
}

void AnnotationSource::loadDescription(FileSource &) {
    loaded = true;
}

} // namespace mbgl
