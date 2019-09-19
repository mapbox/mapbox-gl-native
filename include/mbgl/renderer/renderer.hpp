#pragma once

#include <mbgl/renderer/query.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geojson.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace mbgl {

class RendererObserver;
class RenderedQueryOptions;
class SourceQueryOptions;
class UpdateParameters;

namespace gfx {
class RendererBackend;
} // namespace gfx

class Renderer {
public:
    Renderer(gfx::RendererBackend&, float pixelRatio_,
             const optional<std::string> localFontFamily = {});
    ~Renderer();

    void markContextLost();

    void setObserver(RendererObserver*);

    void render(const UpdateParameters&);

    // Feature queries
    std::vector<Feature> queryRenderedFeatures(const ScreenLineString&, const RenderedQueryOptions& options = {}) const;
    std::vector<Feature> queryRenderedFeatures(const ScreenCoordinate& point, const RenderedQueryOptions& options = {}) const;
    std::vector<Feature> queryRenderedFeatures(const ScreenBox& box, const RenderedQueryOptions& options = {}) const;
    std::vector<Feature> querySourceFeatures(const std::string& sourceID, const SourceQueryOptions& options = {}) const;
    AnnotationIDs queryPointAnnotations(const ScreenBox& box) const;
    AnnotationIDs queryShapeAnnotations(const ScreenBox& box) const;
    AnnotationIDs getAnnotationIDs(const std::vector<Feature>&) const;

    // Feature extension query
    FeatureExtensionValue queryFeatureExtensions(const std::string& sourceID,
                                                 const Feature& feature,
                                                 const std::string& extension,
                                                 const std::string& extensionField,
                                                 const optional<std::map<std::string, Value>>& args = {}) const;

    void setFeatureState(const std::string& sourceID, const optional<std::string>& sourceLayerID,
                         const std::string& featureID, const FeatureState& state);

    void getFeatureState(FeatureState& state, const std::string& sourceID, const optional<std::string>& sourceLayerID,
                         const std::string& featureID) const;

    void removeFeatureState(const std::string& sourceID, const optional<std::string>& sourceLayerID,
                            const optional<std::string>& featureID, const optional<std::string>& stateKey);

    // Debug
    void dumpDebugLogs();

    // Memory
    void reduceMemoryUse();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace mbgl
