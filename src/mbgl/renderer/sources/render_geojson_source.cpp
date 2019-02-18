#include <mbgl/renderer/sources/render_geojson_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/renderer/tile_parameters.hpp>

#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

#include <mapbox/eternal.hpp>

namespace mbgl {

using namespace style;

namespace {

template<typename T, typename C>
optional<T> getProperty(const C& cont, const typename C::key_type& name) {
    const auto it = cont.find(name);
    if (it == cont.end() || !(it->second.template is<T>())) {
        return nullopt;
    }
    return it->second.template get<T>();
}

using FeatureExtensionGetterPtr = FeatureExtensionValue (*)(std::shared_ptr<style::GeoJSONData>,
                                                            std::uint32_t,
                                                            const optional<std::map<std::string, Value>>&);

FeatureExtensionValue getChildren(std::shared_ptr<style::GeoJSONData> clusterData,
                                  std::uint32_t clusterID,
                                  const optional<std::map<std::string, Value>>&) {
    return clusterData->getChildren(clusterID);
}

FeatureExtensionValue getLeaves(std::shared_ptr<style::GeoJSONData> clusterData,
                                std::uint32_t clusterID,
                                const optional<std::map<std::string, Value>>& args) {
    if (args) {
        const auto limit = getProperty<uint64_t>(*args, "limit");
        const auto offset = getProperty<uint64_t>(*args, "offset");
        // Offset cannot be set without limit.
        if (limit) {
            if (offset) {
                return clusterData->getLeaves(clusterID,
                                              static_cast<std::uint32_t>(*limit),
                                              static_cast<std::uint32_t>(*offset));
            }
            return clusterData->getLeaves(clusterID, static_cast<std::uint32_t>(*limit));
        }
    }

    return clusterData->getLeaves(clusterID);
}

FeatureExtensionValue getClusterExpansionZoom(std::shared_ptr<style::GeoJSONData> clusterData,
                                              std::uint32_t clusterID,
                                              const optional<std::map<std::string, Value>>&) {
    return Value{static_cast<uint64_t>(clusterData->getClusterExpansionZoom(clusterID))};
}

MAPBOX_ETERNAL_CONSTEXPR const auto extensionGetters = mapbox::eternal::hash_map<mapbox::eternal::string, FeatureExtensionGetterPtr>({
    {"children", &getChildren},
    {"leaves", &getLeaves},
    {"expansion-zoom", &getClusterExpansionZoom}
});

}

RenderGeoJSONSource::RenderGeoJSONSource(Immutable<style::GeoJSONSource::Impl> impl_)
    : RenderSource(impl_) {
    tilePyramid.setObserver(this);
}

RenderGeoJSONSource::~RenderGeoJSONSource() = default;

const style::GeoJSONSource::Impl& RenderGeoJSONSource::impl() const {
    return static_cast<const style::GeoJSONSource::Impl&>(*baseImpl);
}

bool RenderGeoJSONSource::isLoaded() const {
    return tilePyramid.isLoaded();
}

void RenderGeoJSONSource::update(Immutable<style::Source::Impl> baseImpl_,
                                 const std::vector<Immutable<Layer::Impl>>& layers,
                                 const bool needsRendering,
                                 const bool needsRelayout,
                                 const TileParameters& parameters) {
    std::swap(baseImpl, baseImpl_);

    enabled = needsRendering;

    auto data_ = impl().getData().lock();

    if (data.lock() != data_) {
        data = data_;
        tilePyramid.reduceMemoryUse();

        if (data_) {
            const uint8_t maxZ = impl().getZoomRange().max;
            for (const auto& pair : tilePyramid.getTiles()) {
                if (pair.first.canonical.z <= maxZ) {
                    static_cast<GeoJSONTile*>(pair.second.get())->updateData(data_->getTile(pair.first.canonical));
                }
            }
        }
    }

    if (!data_) {
        tilePyramid.clearAll();
        return;
    }

    tilePyramid.update(layers,
                       needsRendering,
                       needsRelayout,
                       parameters,
                       SourceType::GeoJSON,
                       util::tileSize,
                       impl().getZoomRange(),
                       optional<LatLngBounds>{},
                       [&, data_] (const OverscaledTileID& tileID) {
                           return std::make_unique<GeoJSONTile>(tileID, impl().id, parameters, data_->getTile(tileID.canonical));
                       });
}

void RenderGeoJSONSource::startRender(PaintParameters& parameters) {
    parameters.clipIDGenerator.update(tilePyramid.getRenderTiles());
    tilePyramid.startRender(parameters);
}

void RenderGeoJSONSource::finishRender(PaintParameters& parameters) {
    tilePyramid.finishRender(parameters);
}

std::vector<std::reference_wrapper<RenderTile>> RenderGeoJSONSource::getRenderTiles() {
    return tilePyramid.getRenderTiles();
}

std::unordered_map<std::string, std::vector<Feature>>
RenderGeoJSONSource::queryRenderedFeatures(const ScreenLineString& geometry,
                                           const TransformState& transformState,
                                           const std::vector<const RenderLayer*>& layers,
                                           const RenderedQueryOptions& options,
                                           const mat4& projMatrix) const {
    return tilePyramid.queryRenderedFeatures(geometry, transformState, layers, options, projMatrix);
}

std::vector<Feature> RenderGeoJSONSource::querySourceFeatures(const SourceQueryOptions& options) const {
    return tilePyramid.querySourceFeatures(options);
}

mapbox::util::variant<Value, FeatureCollection>
RenderGeoJSONSource::queryFeatureExtensions(const Feature& feature,
                                            const std::string& extension,
                                            const std::string& extensionField,
                                            const optional<std::map<std::string, Value>>& args) const {
    if (extension != "supercluster") {
        return {};
    }

    const auto extensionIt = extensionGetters.find(extensionField.c_str());
    if (extensionIt == extensionGetters.end()) {
        return {};
    }

    const auto clusterID = getProperty<uint64_t>(feature.properties, "cluster_id");
    if (!clusterID) {
        return {};
    }

    auto jsonData = data.lock();
    if (!jsonData) {
        return {};
    }

    return extensionIt->second(std::move(jsonData), static_cast<std::uint32_t>(*clusterID), args);
}

void RenderGeoJSONSource::reduceMemoryUse() {
    tilePyramid.reduceMemoryUse();
}

void RenderGeoJSONSource::dumpDebugLogs() const {
    tilePyramid.dumpDebugLogs();
}

} // namespace mbgl
