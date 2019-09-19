#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/style/source_impl.hpp>

namespace mbgl {

class AnnotationSource final : public style::Source {
public:
    AnnotationSource();

    class Impl;
    const Impl& impl() const;

private:
    void loadDescription(FileSource&) final;
    bool supportsLayerType(const mbgl::style::LayerTypeInfo*) const override;
    mapbox::base::WeakPtr<Source> makeWeakPtr() override {
        return weakFactory.makeWeakPtr();
    }
    Mutable<Impl> mutableImpl() const;
    mapbox::base::WeakPtrFactory<Source> weakFactory {this};
};

class AnnotationSource::Impl : public style::Source::Impl {
public:
    Impl();

    optional<std::string> getAttribution() const final;
};

} // namespace mbgl
