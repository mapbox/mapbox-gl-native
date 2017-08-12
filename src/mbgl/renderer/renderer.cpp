#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_impl.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/annotation/annotation_manager.hpp>

namespace mbgl {

Renderer::Renderer(RendererBackend& backend,
                   float pixelRatio_,
                   FileSource& fileSource_,
                   Scheduler& scheduler_,
                   GLContextMode contextMode_,
                   const optional<std::string> programCacheDir_)
        : impl(std::make_unique<Impl>(backend, pixelRatio_, fileSource_, scheduler_,
                                      contextMode_, std::move(programCacheDir_))) {
}

Renderer::~Renderer() {
    BackendScope guard { impl->backend };
    impl.reset();
}

void Renderer::setObserver(RendererObserver* observer) {
    impl->setObserver(observer);
}

void Renderer::render(const UpdateParameters& updateParameters) {
    impl->render(updateParameters);
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
    RenderedQueryOptions options;
    options.layerIDs = {{ AnnotationManager::PointLayerID }};
    auto features = queryRenderedFeatures(box, options);
    std::set<AnnotationID> set;
    for (auto &feature : features) {
        assert(feature.id);
        assert(feature.id->is<uint64_t>());
        assert(feature.id->get<uint64_t>() <= std::numeric_limits<AnnotationID>::max());
        set.insert(static_cast<AnnotationID>(feature.id->get<uint64_t>()));
    }
    AnnotationIDs ids;
    ids.reserve(set.size());
    std::move(set.begin(), set.end(), std::back_inserter(ids));
    return ids;
}

std::vector<Feature> Renderer::querySourceFeatures(const std::string& sourceID, const SourceQueryOptions& options) const {
    return impl->querySourceFeatures(sourceID, options);
}

void Renderer::dumpDebugLogs() {
    impl->dumDebugLogs();
}

void Renderer::onLowMemory() {
    BackendScope guard { impl->backend };
    impl->onLowMemory();
}

} // namespace mbgl
