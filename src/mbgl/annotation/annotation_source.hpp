#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/style/source_impl.hpp>

namespace mbgl {

class AnnotationSource : public style::Source {
public:
    AnnotationSource();

    class Impl;
};

class AnnotationSource::Impl : public style::Source::Impl {
public:
    Impl(Source&);

    void loadDescription(FileSource&) final;
    std::unique_ptr<RenderSource> createRenderSource() const final;
};

} // namespace mbgl
