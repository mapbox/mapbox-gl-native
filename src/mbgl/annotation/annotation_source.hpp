#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/style/source_impl.hpp>

#include <memory>

namespace mbgl {

class FileSource;

class AnnotationSource : public style::Source {
public:
    AnnotationSource();

    class Impl;
    const Impl& impl() const;

private:
    void loadDescription(std::shared_ptr<FileSource>) final;

    Mutable<Impl> mutableImpl() const;
};

class AnnotationSource::Impl : public style::Source::Impl {
public:
    Impl();

    optional<std::string> getAttribution() const final;
};

} // namespace mbgl
