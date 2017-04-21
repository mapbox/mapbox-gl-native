#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/render_annotation_source.hpp>

namespace mbgl {

using namespace style;

AnnotationSource::AnnotationSource()
    : Source(SourceType::Annotations, std::make_unique<Impl>(*this)) {
}

AnnotationSource::Impl::Impl(Source& base_)
    : Source::Impl(SourceType::Annotations, AnnotationManager::SourceID, base_) {
}

void AnnotationSource::Impl::loadDescription(FileSource&) {
    loaded = true;
}

std::unique_ptr<RenderSource> AnnotationSource::Impl::createRenderSource() const {
    return std::make_unique<RenderAnnotationSource>(*this);
}

} // namespace mbgl
