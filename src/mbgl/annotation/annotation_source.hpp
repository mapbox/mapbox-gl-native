#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/style/source_impl.hpp>

namespace mbgl {

class AnnotationSource final : public style::Source {
public:
    AnnotationSource();

    class Impl;
    const Impl& impl() const;

    mapbox::base::WeakPtr<Source> makeWeakPtr() override {
        return weakFactory.makeWeakPtr();
    }

private:
    void loadDescription(FileSource&) final;

    Mutable<Impl> mutableImpl() const;
    mapbox::base::WeakPtrFactory<Source> weakFactory {this};
};

class AnnotationSource::Impl : public style::Source::Impl {
public:
    Impl();

    optional<std::string> getAttribution() const final;
};

} // namespace mbgl
