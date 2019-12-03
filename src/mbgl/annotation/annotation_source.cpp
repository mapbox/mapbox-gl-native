#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/style/layer.hpp>

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

bool AnnotationSource::supportsLayerType(const mbgl::style::LayerTypeInfo* info) const {
    return !std::strcmp(info->type, "line") || !std::strcmp(info->type, "symbol") || !std::strcmp(info->type, "fill");
}

} // namespace mbgl
