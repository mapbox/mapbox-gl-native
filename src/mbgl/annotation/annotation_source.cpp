#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/render_annotation_source.hpp>

namespace mbgl {

using namespace style;

AnnotationSource::AnnotationSource()
    : Source(makeMutable<Impl>()) {
}

AnnotationSource::Impl::Impl()
    : Source::Impl(SourceType::Annotations, AnnotationManager::SourceID) {
}

void AnnotationSource::loadDescription(FileSource&) {
    loaded = true;
}

optional<std::string> AnnotationSource::Impl::getAttribution() const {
    return {};
}

std::unique_ptr<RenderSource> AnnotationSource::Impl::createRenderSource() const {
    return std::make_unique<RenderAnnotationSource>(staticImmutableCast<AnnotationSource::Impl>(immutableFromThis()));
}

} // namespace mbgl
