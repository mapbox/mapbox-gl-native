#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/offline_download.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/http_file_source.hpp>
#include <mbgl/style/parser.hpp>
#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/raster_dem_source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/tileset.hpp>

#include <set>

namespace {

const size_t kResourcesBatchSize = 64;
const size_t kMarkBatchSize = 200;

} // namespace

namespace mbgl {

using namespace style;

// Generic functions

template <class RegionDefinition>
Range<uint8_t> coveringZoomRange(const RegionDefinition& definition,
                                 style::SourceType type, uint16_t tileSize, const Range<uint8_t>& zoomRange) {
    double minZ = std::max<double>(util::coveringZoomLevel(definition.minZoom, type, tileSize), zoomRange.min);
    double maxZ = std::min<double>(util::coveringZoomLevel(definition.maxZoom, type, tileSize), zoomRange.max);

    assert(minZ >= 0);
    assert(maxZ >= 0);
    assert(minZ < std::numeric_limits<uint8_t>::max());
    assert(maxZ < std::numeric_limits<uint8_t>::max());
    return { static_cast<uint8_t>(minZ), static_cast<uint8_t>(maxZ) };
}

template <class Geometry, class Fn>
void tileCover(const Geometry& geometry, uint8_t z, Fn&& fn) {
    util::TileCover cover(geometry, z);
    while (cover.hasNext()) {
        fn(cover.next()->canonical);
    }
}


template <class Fn>
void tileCover(const OfflineRegionDefinition& definition, style::SourceType type,
               uint16_t tileSize, const Range<uint8_t>& zoomRange, Fn&& fn) {
    const Range<uint8_t> clampedZoomRange =
            definition.match([&](auto& reg) { return coveringZoomRange(reg, type, tileSize, zoomRange); });

    for (uint8_t z = clampedZoomRange.min; z <= clampedZoomRange.max; z++) {
        definition.match(
                [&](const OfflineTilePyramidRegionDefinition& reg){ tileCover(reg.bounds, z, fn); },
                [&](const OfflineGeometryRegionDefinition& reg){ tileCover(reg.geometry, z, fn); }
        );
    }
}

uint64_t tileCount(const OfflineRegionDefinition& definition, style::SourceType type,
                   uint16_t tileSize, const Range<uint8_t>& zoomRange) {

    const Range<uint8_t> clampedZoomRange =
            definition.match([&](auto& reg) { return coveringZoomRange(reg, type, tileSize, zoomRange); });

    unsigned long result = 0;;
    for (uint8_t z = clampedZoomRange.min; z <= clampedZoomRange.max; z++) {
        result += definition.match(
                [&](const OfflineTilePyramidRegionDefinition& reg){ return util::tileCount(reg.bounds, z); },
                [&](const OfflineGeometryRegionDefinition& reg){ return util::tileCount(reg.geometry, z); }
        );
    }

    return result;
}

// OfflineDownload

OfflineDownload::OfflineDownload(int64_t id_,
                                 OfflineRegionDefinition&& definition_,
                                 OfflineDatabase& offlineDatabase_,
                                 OnlineFileSource& onlineFileSource_)
    : id(id_),
      definition(definition_),
      offlineDatabase(offlineDatabase_),
      onlineFileSource(onlineFileSource_) {
    setObserver(nullptr);
}

OfflineDownload::~OfflineDownload() = default;

void OfflineDownload::setObserver(std::unique_ptr<OfflineRegionObserver> observer_) {
    observer = observer_ ? std::move(observer_) : std::make_unique<OfflineRegionObserver>();
}

void OfflineDownload::setState(OfflineRegionDownloadState state) {
    if (status.downloadState == state) {
        return;
    }

    status.downloadState = state;

    if (status.downloadState == OfflineRegionDownloadState::Active) {
        activateDownload();
    } else {
        deactivateDownload();
    }

    observer->statusChanged(status);
}

OfflineRegionStatus OfflineDownload::getStatus() const {
    if (status.downloadState == OfflineRegionDownloadState::Active) {
        return status;
    }

    auto result = offlineDatabase.getRegionCompletedStatus(id);
    if (!result) {
        // We can't find this offline region because the database is unavailable, or the download
        // does not exist.
        return {};
    }

    result->requiredResourceCount++;
    optional<Response> styleResponse =
            offlineDatabase.get(Resource::style(definition.match([](auto& reg){ return reg.styleURL; })));
    if (!styleResponse) {
        return *result;
    }

    style::Parser parser;
    parser.parse(*styleResponse->data);

    result->requiredResourceCountIsPrecise = true;

    for (const auto& source : parser.sources) {
        SourceType type = source->getType();

        auto handleTiledSource = [&] (const variant<std::string, Tileset>& urlOrTileset, const uint16_t tileSize) {
            if (urlOrTileset.is<Tileset>()) {
                uint64_t tileSourceCount = tileCount(definition, type, tileSize, urlOrTileset.get<Tileset>().zoomRange);
                result->requiredTileCount += tileSourceCount;
                result->requiredResourceCount += tileSourceCount;
            } else {
                result->requiredResourceCount += 1;
                const auto& url = urlOrTileset.get<std::string>();
                optional<Response> sourceResponse = offlineDatabase.get(Resource::source(url));
                if (sourceResponse) {
                    style::conversion::Error error;
                    optional<Tileset> tileset = style::conversion::convertJSON<Tileset>(*sourceResponse->data, error);
                    if (tileset) {
                        uint64_t tileSourceCount = tileCount(definition, type, tileSize, (*tileset).zoomRange);
                        result->requiredTileCount += tileSourceCount;
                        result->requiredResourceCount += tileSourceCount;
                    }
                } else {
                    result->requiredResourceCountIsPrecise = false;
                }
            }
        };

        switch (type) {
        case SourceType::Vector: {
            const auto& vectorSource = *source->as<VectorSource>();
            handleTiledSource(vectorSource.getURLOrTileset(), util::tileSize);
            break;
        }

        case SourceType::Raster: {
            const auto& rasterSource = *source->as<RasterSource>();
            handleTiledSource(rasterSource.getURLOrTileset(), rasterSource.getTileSize());
            break;
        }

        case SourceType::RasterDEM: {
            const auto& rasterDEMSource = *source->as<RasterDEMSource>();
            handleTiledSource(rasterDEMSource.getURLOrTileset(), rasterDEMSource.getTileSize());
            break;
        }

        case SourceType::GeoJSON: {
            const auto& geojsonSource = *source->as<GeoJSONSource>();
            if (geojsonSource.getURL()) {
                result->requiredResourceCount += 1;
            }
            break;
        }

        case SourceType::Image: {
            const auto& imageSource = *source->as<ImageSource>();
            if (imageSource.getURL()) {
                result->requiredResourceCount += 1;
            }
            break;
        }

        case SourceType::Video:
        case SourceType::Annotations:
        case SourceType::CustomVector:
            break;
        }
    }

    if (!parser.glyphURL.empty()) {
        result->requiredResourceCount += parser.fontStacks().size() *
            (definition.match([](auto& reg){ return reg.includeIdeographs; }) ?
                GLYPH_RANGES_PER_FONT_STACK :
                NON_IDEOGRAPH_GLYPH_RANGES_PER_FONT_STACK);
    }

    if (!parser.spriteURL.empty()) {
        result->requiredResourceCount += 4;
    }

    return *result;
}

void OfflineDownload::activateDownload() {
    status = OfflineRegionStatus();
    status.downloadState = OfflineRegionDownloadState::Active;
    status.requiredResourceCount++;

    auto styleResource = Resource::style(definition.match([](auto& reg){ return reg.styleURL; }));
    styleResource.setPriority(Resource::Priority::Low);
    styleResource.setUsage(Resource::Usage::Offline);

    ensureResource(std::move(styleResource), [&](Response styleResponse) {
        status.requiredResourceCountIsPrecise = true;

        style::Parser parser;
        parser.parse(*styleResponse.data);

        for (const auto& source : parser.sources) {
            SourceType type = source->getType();

            auto handleTiledSource = [&] (const variant<std::string, Tileset>& urlOrTileset, const uint16_t tileSize) {
                if (urlOrTileset.is<Tileset>()) {
                    queueTiles(type, tileSize, urlOrTileset.get<Tileset>());
                } else {
                    const auto& url = urlOrTileset.get<std::string>();
                    status.requiredResourceCountIsPrecise = false;
                    status.requiredResourceCount++;
                    requiredSourceURLs.insert(url);

                    auto sourceResource = Resource::source(url);
                    sourceResource.setPriority(Resource::Priority::Low);
                    sourceResource.setUsage(Resource::Usage::Offline);

                    ensureResource(std::move(sourceResource), [=](Response sourceResponse) {
                        style::conversion::Error error;
                        optional<Tileset> tileset = style::conversion::convertJSON<Tileset>(*sourceResponse.data, error);
                        if (tileset) {
                            util::mapbox::canonicalizeTileset(*tileset, url, type, tileSize);
                            queueTiles(type, tileSize, *tileset);

                            requiredSourceURLs.erase(url);
                            if (requiredSourceURLs.empty()) {
                                status.requiredResourceCountIsPrecise = true;
                            }
                        }
                    });
                }
            };

            switch (type) {
            case SourceType::Vector: {
                const auto& vectorSource = *source->as<VectorSource>();
                handleTiledSource(vectorSource.getURLOrTileset(), util::tileSize);
                break;
            }

            case SourceType::Raster: {
                const auto& rasterSource = *source->as<RasterSource>();
                handleTiledSource(rasterSource.getURLOrTileset(), rasterSource.getTileSize());
                break;
            }

            case SourceType::RasterDEM: {
                const auto& rasterDEMSource = *source->as<RasterDEMSource>();
                handleTiledSource(rasterDEMSource.getURLOrTileset(), rasterDEMSource.getTileSize());
                break;
            }

            case SourceType::GeoJSON: {
                const auto& geojsonSource = *source->as<GeoJSONSource>();
                if (geojsonSource.getURL()) {
                    queueResource(Resource::source(*geojsonSource.getURL()));
                }
                break;
            }

            case SourceType::Image: {
                const auto& imageSource = *source->as<ImageSource>();
                auto imageUrl = imageSource.getURL();
                if (imageUrl && !imageUrl->empty()) {
                    queueResource(Resource::image(*imageUrl));
                }
                break;
            }

            case SourceType::Video:
            case SourceType::Annotations:
            case SourceType::CustomVector:
                break;
            }
        }

        if (!parser.glyphURL.empty()) {
            const bool includeIdeographs = definition.match([](auto& reg){ return reg.includeIdeographs; });
            for (const auto& fontStack : parser.fontStacks()) {
                for (char16_t i = 0; i < GLYPH_RANGES_PER_FONT_STACK; i++) {
                    // Assumes that if a glyph range starts with fixed width/ideographic characters, the entire
                    // range will be fixed width.
                    if (includeIdeographs || !util::i18n::allowsFixedWidthGlyphGeneration(i * GLYPHS_PER_GLYPH_RANGE)) {
                        queueResource(Resource::glyphs(parser.glyphURL, fontStack, getGlyphRange(i * GLYPHS_PER_GLYPH_RANGE)));
                    }
                }
            }
        }

        if (!parser.spriteURL.empty()) {
            // Always request 1x and @2x sprite images for portability.
            queueResource(Resource::spriteImage(parser.spriteURL, 1));
            queueResource(Resource::spriteImage(parser.spriteURL, 2));
            queueResource(Resource::spriteJSON(parser.spriteURL, 1));
            queueResource(Resource::spriteJSON(parser.spriteURL, 2));
        }

        continueDownload();
    });
}

/*
   Fill up our own request queue by requesting the next few resources. This is called
   when activating the download, or when a request completes successfully.

   Note "successfully"; it's not called when a requests receives an error. A request
   that errors will be retried after some delay. So in that sense it's still "active"
   and consuming resources, notably the request object, its timer, and network resources
   when the timer fires.

   We could try to squeeze in subsequent requests while we wait for the errored request
   to retry. But that risks overloading the upstream request queue -- defeating our own
   metering -- if there are a lot of errored requests that all come up for retry at the
   same time. And many times, the cause of a request error will apply to many requests
   of the same type. For instance if a server is unreachable, all the requests to that
   host are going to error. In that case, continuing to try subsequent resources after
   the first few errors is fruitless anyway.
*/
void OfflineDownload::continueDownload() {
    if (resourcesRemaining.empty() && status.complete()) {
        markPendingUsedResources();
        setState(OfflineRegionDownloadState::Inactive);
        return;
    }

    if (resourcesToBeMarkedAsUsed.size() >= kMarkBatchSize) markPendingUsedResources();

    while (!resourcesRemaining.empty() && requests.size() < onlineFileSource.getMaximumConcurrentRequests()) {
        ensureResource(std::move(resourcesRemaining.front()));
        resourcesRemaining.pop_front();
    }
}

void OfflineDownload::deactivateDownload() {
    requiredSourceURLs.clear();
    resourcesRemaining.clear();
    requests.clear();
    buffer.clear();
}

void OfflineDownload::queueResource(Resource&& resource) {
    resource.setPriority(Resource::Priority::Low);
    resource.setUsage(Resource::Usage::Offline);
    status.requiredResourceCount++;
    if (resource.kind == mbgl::Resource::Kind::Tile) {
        status.requiredTileCount++;
    }
    resourcesRemaining.push_front(std::move(resource));
}

void OfflineDownload::queueTiles(SourceType type, uint16_t tileSize, const Tileset& tileset) {
    tileCover(definition, type, tileSize, tileset.zoomRange, [&](const auto& tile) {
        status.requiredResourceCount++;
        status.requiredTileCount++;

        auto tileResource = Resource::tile(
                tileset.tiles[0], definition.match([](auto& def) { return def.pixelRatio; }),
                tile.x, tile.y, tile.z, tileset.scheme);

        tileResource.setPriority(Resource::Priority::Low);
        tileResource.setUsage(Resource::Usage::Offline);

        resourcesRemaining.push_back(std::move(tileResource));
    });
}

void OfflineDownload::markPendingUsedResources() {
    offlineDatabase.markUsedResources(id, resourcesToBeMarkedAsUsed);
    resourcesToBeMarkedAsUsed.clear();
}

void OfflineDownload::ensureResource(Resource&& resource,
                                     std::function<void(Response)> callback) {
    assert(resource.priority == Resource::Priority::Low);
    assert(resource.usage == Resource::Usage::Offline);

    auto workRequestsIt = requests.insert(requests.begin(), nullptr);
    *workRequestsIt = util::RunLoop::Get()->invokeCancellable([=]() {
        requests.erase(workRequestsIt);
        const auto resourceKind = resource.kind;
        auto getResourceSizeInDatabase = [&] () -> optional<int64_t> {
            optional<int64_t> result;
            if (!callback) {
                result = offlineDatabase.hasRegionResource(resource);
            } else {
                optional<std::pair<Response, uint64_t>> response = offlineDatabase.getRegionResource(resource);
                if (response) {
                    callback(response->first);
                    result = response->second;
                }
            }

            if (result) resourcesToBeMarkedAsUsed.emplace_back(resource);
            return result;
        };

        optional<int64_t> offlineResponse = getResourceSizeInDatabase();
        if (offlineResponse) {
            assert(!resourcesToBeMarkedAsUsed.empty());
            status.completedResourceCount++;
            status.completedResourceSize += *offlineResponse;
            if (resourceKind == Resource::Kind::Tile) {
                status.completedTileCount += 1;
                status.completedTileSize += *offlineResponse;
            }

            observer->statusChanged(status);
            continueDownload();
            return;
        }

        if (offlineDatabase.exceedsOfflineMapboxTileCountLimit(resource)) {
            onMapboxTileCountLimitExceeded();
            return;
        }

        auto fileRequestsIt = requests.insert(requests.begin(), nullptr);
        *fileRequestsIt = onlineFileSource.request(resource, [=](Response onlineResponse) {
            if (onlineResponse.error) {
                observer->responseError(*onlineResponse.error);
                return;
            }

            requests.erase(fileRequestsIt);

            if (callback) {
                callback(onlineResponse);
            }

            // Queue up for batched insertion
            buffer.emplace_back(resource, onlineResponse);

            // Flush buffer periodically
            if (buffer.size() == kResourcesBatchSize || resourcesRemaining.empty()) {
                try {
                    offlineDatabase.putRegionResources(id, buffer, status);
                } catch (const MapboxTileLimitExceededException&) {
                    onMapboxTileCountLimitExceeded();
                    return;
                }

                buffer.clear();
                observer->statusChanged(status);
            }

            if (offlineDatabase.exceedsOfflineMapboxTileCountLimit(resource)) {
                onMapboxTileCountLimitExceeded();
                return;
            }

            continueDownload();
        });
    });
}

void OfflineDownload::onMapboxTileCountLimitExceeded() {
    observer->mapboxTileCountLimitExceeded(offlineDatabase.getOfflineMapboxTileCountLimit());
    setState(OfflineRegionDownloadState::Inactive);
}

} // namespace mbgl
