#include <mbgl/storage/offline_download.hpp>
#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/style/style_parser.hpp>
#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/util/tile_cover.hpp>

#include <set>

namespace mbgl {

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
}

std::vector<Resource> OfflineDownload::spriteResources(const StyleParser& parser) const {
    std::vector<Resource> result;

    if (!parser.spriteURL.empty()) {
        result.push_back(Resource::spriteImage(parser.spriteURL, definition.pixelRatio));
        result.push_back(Resource::spriteJSON(parser.spriteURL, definition.pixelRatio));
    }

    return result;
}

std::vector<Resource> OfflineDownload::glyphResources(const StyleParser& parser) const {
    std::vector<Resource> result;

    if (!parser.glyphURL.empty()) {
        for (const auto& fontStack : parser.fontStacks()) {
            for (uint32_t i = 0; i < 256; i++) {
                result.push_back(Resource::glyphs(parser.glyphURL, fontStack, getGlyphRange(i * 256)));
            }
        }
    }

    return result;
}

std::vector<Resource> OfflineDownload::tileResources(SourceType type, uint16_t tileSize, const SourceInfo& info) const {
    std::vector<Resource> result;

    for (const auto& tile : definition.tileCover(type, tileSize, info)) {
        result.push_back(Resource::tile(info.tiles[0], definition.pixelRatio, tile.x, tile.y, tile.z));
    }

    return result;
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

    StyleParser parser;
    parser.parse(*styleResponse->data);

    result.requiredResourceCountIsIndeterminate = false;

    for (const auto& source : parser.sources) {
        switch (source->type) {
        case SourceType::Vector:
        case SourceType::Raster:
            if (source->getInfo()) {
                result.requiredResourceCount += tileResources(source->type, source->tileSize, *source->getInfo()).size();
            } else {
                result.requiredResourceCount += 1;
                optional<Response> sourceResponse = offlineDatabase.get(Resource::source(source->url));
                if (sourceResponse) {
                    result.requiredResourceCount += tileResources(source->type, source->tileSize,
                        *StyleParser::parseTileJSON(*sourceResponse->data, source->url, source->type)).size();
                } else {
                    result.requiredResourceCountIsIndeterminate = true;
                }
            }
            break;

        case SourceType::GeoJSON:
            if (!source->url.empty()) {
                result.requiredResourceCount += 1;
            }
            break;

        case SourceType::Video:
        case SourceType::Annotations:
            break;
        }
    }

    result.requiredResourceCount += spriteResources(parser).size();
    result.requiredResourceCount += glyphResources(parser).size();

    return result;
}

void OfflineDownload::activateDownload() {
    status = offlineDatabase.getRegionCompletedStatus(id);
    requiredSourceURLs.clear();

    ensureResource(Resource::style(definition.styleURL), [&] (Response styleResponse) {
        status.requiredResourceCountIsIndeterminate = false;

        StyleParser parser;
        parser.parse(*styleResponse.data);

        for (const auto& source : parser.sources) {
            SourceType type = source->type;
            uint16_t tileSize = source->tileSize;
            std::string url = source->url;

            switch (type) {
            case SourceType::Vector:
            case SourceType::Raster:
                if (source->getInfo()) {
                    ensureTiles(type, tileSize, *source->getInfo());
                } else {
                    status.requiredResourceCountIsIndeterminate = true;
                    requiredSourceURLs.insert(url);

                    ensureResource(Resource::source(url), [=] (Response sourceResponse) {
                        ensureTiles(type, tileSize, *StyleParser::parseTileJSON(*sourceResponse.data, url, type));

                        requiredSourceURLs.erase(url);
                        if (requiredSourceURLs.empty()) {
                            status.requiredResourceCountIsIndeterminate = false;
                        }
                    });
                }
                break;

            case SourceType::GeoJSON:
                if (!source->url.empty()) {
                    ensureResource(Resource::source(source->url));
                }
                break;

            case SourceType::Video:
            case SourceType::Annotations:
                break;
            }
        }
        
        for (const auto& resource : spriteResources(parser)) {
            ensureResource(resource);
        }

        for (const auto& resource : glyphResources(parser)) {
            ensureResource(resource);
        }
    });

    // This will be the initial notification, after we've incremented requiredResourceCount
    // to the reflect the extent to which required resources are already in the database.
    observer->statusChanged(status);
}

void OfflineDownload::deactivateDownload() {
    requests.clear();
}

void OfflineDownload::ensureTiles(SourceType type, uint16_t tileSize, const SourceInfo& info) {
    for (const auto& resource : tileResources(type, tileSize, info)) {
        ensureResource(resource);
    }
}

void OfflineDownload::ensureResource(const Resource& resource, std::function<void (Response)> callback) {
    status.requiredResourceCount++;

    optional<Response> offlineResponse = offlineDatabase.getRegionResource(id, resource);
    if (offlineResponse) {
        if (callback) {
            callback(*offlineResponse);
        }

        // Not incrementing status.completedResource{Size,Count} here because previously-existing
        // resources are already accounted for by offlineDatabase.getRegionCompletedStatus();

        return;
    }

    auto it = requests.insert(requests.begin(), nullptr);
    *it = onlineFileSource.request(resource, [=] (Response onlineResponse) {
        requests.erase(it);

        if (onlineResponse.error) {
            observer->responseError(*onlineResponse.error);
            return;
        }

        if (callback) {
            callback(onlineResponse);
        }

        status.completedResourceCount++;
        status.completedResourceSize += offlineDatabase.putRegionResource(id, resource, onlineResponse);

        observer->statusChanged(status);
    });
}

} // namespace mbgl
