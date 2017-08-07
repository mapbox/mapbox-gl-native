#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/annotation/annotation_manager.hpp>

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

} // namespace mbgl
