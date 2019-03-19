#include <mbgl/renderer/renderer.hpp>

#include <mbgl/layermanager/layer_manager.hpp>
#include <mbgl/renderer/renderer_impl.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/annotation/annotation_manager.hpp>

namespace mbgl {

Renderer::Renderer(RendererBackend& backend,
                   float pixelRatio_,
                   Scheduler& scheduler_,
                   GLContextMode contextMode_,
                   const optional<std::string> programCacheDir_,
                   const optional<std::string> localFontFamily_)
        : impl(std::make_unique<Impl>(backend, pixelRatio_, scheduler_,
                                      contextMode_, std::move(programCacheDir_), std::move(localFontFamily_))) {
}

Renderer::~Renderer() {
    BackendScope guard { impl->backend };
    impl.reset();
}

void Renderer::markContextLost() {
    impl->markContextLost();
}

void Renderer::setObserver(RendererObserver* observer) {
    impl->setObserver(observer);
}

void Renderer::render(const UpdateParameters& updateParameters) {
    impl->render(updateParameters);
}

void Renderer::flush() {
    impl->flush();
}

std::vector<Feature> Renderer::queryRenderedFeatures(const ScreenLineString& geometry, const RenderedQueryOptions& options) const {
    return impl->queryRenderedFeatures(geometry, options);
}

std::vector<Feature> Renderer::queryRenderedFeatures(const ScreenCoordinate& point, const RenderedQueryOptions& options) const {
    return impl->queryRenderedFeatures({ point }, options);
}

std::vector<Feature> Renderer::queryRenderedFeatures(const ScreenBox& box, const RenderedQueryOptions& options) const {
    return impl->queryRenderedFeatures(
            {
                    box.min,
                    {box.max.x, box.min.y},
                    box.max,
                    {box.min.x, box.max.y},
                    box.min
            },
            options
    );
}

AnnotationIDs Renderer::queryPointAnnotations(const ScreenBox& box) const {
    if (!LayerManager::annotationsEnabled) {
        return {};
    }
    RenderedQueryOptions options;
    options.layerIDs = {{ AnnotationManager::PointLayerID }};
    auto features = queryRenderedFeatures(box, options);
    return getAnnotationIDs(features);
}

AnnotationIDs Renderer::queryShapeAnnotations(const ScreenBox& box) const {
    if (!LayerManager::annotationsEnabled) {
        return {};
    }
    auto features = impl->queryShapeAnnotations({
        box.min,
        {box.max.x, box.min.y},
        box.max,
        {box.min.x, box.max.y},
        box.min
    });
    return getAnnotationIDs(features);
}
    
AnnotationIDs Renderer::getAnnotationIDs(const std::vector<Feature>& features) const {
    if (!LayerManager::annotationsEnabled) {
        return {};
    }
    std::set<AnnotationID> set;
    for (auto &feature : features) {
        assert(feature.id.is<uint64_t>());
        assert(feature.id.get<uint64_t>() <= std::numeric_limits<AnnotationID>::max());
        set.insert(static_cast<AnnotationID>(feature.id.get<uint64_t>()));
    }
    AnnotationIDs ids;
    ids.reserve(set.size());
    std::move(set.begin(), set.end(), std::back_inserter(ids));
    return ids;
}

std::vector<Feature> Renderer::querySourceFeatures(const std::string& sourceID, const SourceQueryOptions& options) const {
    return impl->querySourceFeatures(sourceID, options);
}

FeatureExtensionValue Renderer::queryFeatureExtensions(const std::string& sourceID,
                                                       const Feature& feature,
                                                       const std::string& extension,
                                                       const std::string& extensionField,
                                                       const optional<std::map<std::string, Value>>& args) const {
    return impl->queryFeatureExtensions(sourceID, feature, extension, extensionField, args);
}

void Renderer::dumpDebugLogs() {
    impl->dumDebugLogs();
}

void Renderer::reduceMemoryUse() {
    BackendScope guard { impl->backend };
    impl->reduceMemoryUse();
}

} // namespace mbgl
