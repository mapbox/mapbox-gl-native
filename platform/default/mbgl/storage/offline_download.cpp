#include <mbgl/storage/file_source.hpp>
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
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/tileset.hpp>

#include <set>

namespace mbgl {

using namespace style;

OfflineDownload::OfflineDownload(int64_t id_,
                                 OfflineRegionDefinition&& definition_,
                                 OfflineDatabase& offlineDatabase_,
                                 FileSource& onlineFileSource_)
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

    OfflineRegionStatus result = offlineDatabase.getRegionCompletedStatus(id);

    result.requiredResourceCount++;
    optional<Response> styleResponse = offlineDatabase.get(Resource::style(definition.styleURL));
    if (!styleResponse) {
        return result;
    }

    style::Parser parser;
    parser.parse(*styleResponse->data);

    result.requiredResourceCountIsPrecise = true;

    for (const auto& source : parser.sources) {
        SourceType type = source->getType();

        auto handleTiledSource = [&] (const variant<std::string, Tileset>& urlOrTileset, const uint16_t tileSize) {
            if (urlOrTileset.is<Tileset>()) {
                result.requiredResourceCount +=
                    definition.tileCount(type, tileSize, urlOrTileset.get<Tileset>().zoomRange);
            } else {
                result.requiredResourceCount += 1;
                const auto& url = urlOrTileset.get<std::string>();
                optional<Response> sourceResponse = offlineDatabase.get(Resource::source(url));
                if (sourceResponse) {
                    style::conversion::Error error;
                    optional<Tileset> tileset = style::conversion::convertJSON<Tileset>(*sourceResponse->data, error);
                    if (tileset) {
                        result.requiredResourceCount +=
                            definition.tileCount(type, tileSize, (*tileset).zoomRange);
                    }
                } else {
                    result.requiredResourceCountIsPrecise = false;
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
                result.requiredResourceCount += 1;
            }
            break;
        }

        case SourceType::Image: {
            const auto& imageSource = *source->as<ImageSource>();
            if (imageSource.getURL()) {
                result.requiredResourceCount += 1;
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
        result.requiredResourceCount += parser.fontStacks().size() * GLYPH_RANGES_PER_FONT_STACK;
    }

    if (!parser.spriteURL.empty()) {
        result.requiredResourceCount += 2;
    }

    return result;
}

void OfflineDownload::activateDownload() {
    status = OfflineRegionStatus();
    status.downloadState = OfflineRegionDownloadState::Active;
    status.requiredResourceCount++;
    ensureResource(Resource::style(definition.styleURL), [&](Response styleResponse) {
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

                    ensureResource(Resource::source(url), [=](Response sourceResponse) {
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
            for (const auto& fontStack : parser.fontStacks()) {
                for (char16_t i = 0; i < GLYPH_RANGES_PER_FONT_STACK; i++) {
                    queueResource(Resource::glyphs(parser.glyphURL, fontStack, getGlyphRange(i * GLYPHS_PER_GLYPH_RANGE)));
                }
            }
        }

        if (!parser.spriteURL.empty()) {
            queueResource(Resource::spriteImage(parser.spriteURL, definition.pixelRatio));
            queueResource(Resource::spriteJSON(parser.spriteURL, definition.pixelRatio));
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
        setState(OfflineRegionDownloadState::Inactive);
        return;
    }

    while (!resourcesRemaining.empty() && requests.size() < HTTPFileSource::maximumConcurrentRequests()) {
        ensureResource(resourcesRemaining.front());
        resourcesRemaining.pop_front();
    }
}

void OfflineDownload::deactivateDownload() {
    requiredSourceURLs.clear();
    resourcesRemaining.clear();
    requests.clear();
}

void OfflineDownload::queueResource(Resource resource) {
    status.requiredResourceCount++;
    resourcesRemaining.push_front(std::move(resource));
}

void OfflineDownload::queueTiles(SourceType type, uint16_t tileSize, const Tileset& tileset) {
    for (const auto& tile : definition.tileCover(type, tileSize, tileset.zoomRange)) {
        status.requiredResourceCount++;
        resourcesRemaining.push_back(
            Resource::tile(tileset.tiles[0], definition.pixelRatio, tile.x, tile.y, tile.z, tileset.scheme));
    }
}

void OfflineDownload::ensureResource(const Resource& resource,
                                     std::function<void(Response)> callback) {
    auto workRequestsIt = requests.insert(requests.begin(), nullptr);
    *workRequestsIt = util::RunLoop::Get()->invokeCancellable([=]() {
        requests.erase(workRequestsIt);

        auto getResourceSizeInDatabase = [&] () -> optional<int64_t> {
            if (!callback) {
                return offlineDatabase.hasRegionResource(id, resource);
            }
            optional<std::pair<Response, uint64_t>> response = offlineDatabase.getRegionResource(id, resource);
            if (!response) {
                return {};
            }
            callback(response->first);
            return response->second;
        };

        optional<int64_t> offlineResponse = getResourceSizeInDatabase();
        if (offlineResponse) {
            status.completedResourceCount++;
            status.completedResourceSize += *offlineResponse;
            if (resource.kind == Resource::Kind::Tile) {
                status.completedTileCount += 1;
                status.completedTileSize += *offlineResponse;
            }

            observer->statusChanged(status);
            continueDownload();
            return;
        }

        if (checkTileCountLimit(resource)) {
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

            status.completedResourceCount++;
            uint64_t resourceSize = offlineDatabase.putRegionResource(id, resource, onlineResponse);
            status.completedResourceSize += resourceSize;
            if (resource.kind == Resource::Kind::Tile) {
                status.completedTileCount += 1;
                status.completedTileSize += resourceSize;
            }

            observer->statusChanged(status);

            if (checkTileCountLimit(resource)) {
                return;
            }

            continueDownload();
        });
    });
}

bool OfflineDownload::checkTileCountLimit(const Resource& resource) {
    if (resource.kind == Resource::Kind::Tile && util::mapbox::isMapboxURL(resource.url) &&
        offlineDatabase.offlineMapboxTileCountLimitExceeded()) {
        observer->mapboxTileCountLimitExceeded(offlineDatabase.getOfflineMapboxTileCountLimit());
        setState(OfflineRegionDownloadState::Inactive);
        return true;
    }

    return false;
}

} // namespace mbgl
