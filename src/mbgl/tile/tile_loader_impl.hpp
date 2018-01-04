#pragma once

#include <mbgl/tile/tile_loader.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/util/tileset.hpp>

#include <cassert>

namespace mbgl {

template <typename T>
TileLoader<T>::TileLoader(T& tile_,
                          const OverscaledTileID& id,
                          const TileParameters& parameters,
                          const Tileset& tileset)
    : tile(tile_),
      necessity(TileNecessity::Optional),
      resource(Resource::tile(
        tileset.tiles.at(0),
        parameters.pixelRatio,
        id.canonical.x,
        id.canonical.y,
        id.canonical.z,
        tileset.scheme,
        Resource::LoadingMethod::CacheOnly)),
      fileSource(parameters.fileSource) {
    assert(!request);
    if (fileSource.supportsCacheOnlyRequests()) {
        // When supported, the first request is always optional, even if the TileLoader
        // is marked as required. That way, we can let the first optional request continue
        // to load when the TileLoader is later changed from required to optional. If we
        // started out with a required request, we'd have to cancel everything, including the
        // initial optional part of the request.
        loadFromCache();
    } else if (necessity == TileNecessity::Required) {
        // When the file source doesn't support cache-only requests, and we definiitely need this
        // data, we can start out with a network request immediately.
        loadFromNetwork();
    } else {
        // When the FileSource doesn't support cache-only requests, we do nothing until the
        // data is definitely required.
    }
}

template <typename T>
TileLoader<T>::~TileLoader() = default;

template <typename T>
void TileLoader<T>::loadFromCache() {
    assert(!request);

    resource.loadingMethod = Resource::LoadingMethod::CacheOnly;
    request = fileSource.request(resource, [this](Response res) {
        request.reset();

        tile.setTriedCache();

        if (res.error && res.error->reason == Response::Error::Reason::NotFound) {
            // When the cache-only request could not be satisfied, don't treat it as an error.
            // A cache lookup could still return data, _and_ an error, in particular when we were
            // able to find the data, but it is expired and the Cache-Control headers indicated that
            // we aren't allowed to use expired responses. In this case, we still get the data which
            // we can use in our conditional network request.
            resource.priorModified = res.modified;
            resource.priorExpires = res.expires;
            resource.priorEtag = res.etag;
            resource.priorData = res.data;
        } else {
            loadedData(res);
        }

        if (necessity == TileNecessity::Required) {
            loadFromNetwork();
        }
    });
}

template <typename T>
void TileLoader<T>::makeRequired() {
    if (!request) {
        loadFromNetwork();
    }
}

template <typename T>
void TileLoader<T>::makeOptional() {
    if (resource.loadingMethod == Resource::LoadingMethod::NetworkOnly && request) {
        // Abort the current request, but only when we know that we're specifically querying for a
        // network resource only.
        request.reset();
    }
}

template <typename T>
void TileLoader<T>::loadedData(const Response& res) {
    if (res.error && res.error->reason != Response::Error::Reason::NotFound) {
        tile.setError(std::make_exception_ptr(std::runtime_error(res.error->message)));
    } else if (res.notModified) {
        resource.priorExpires = res.expires;
        // Do not notify the tile; when we get this message, it already has the current
        // version of the data.
        tile.setMetadata(res.modified, res.expires);
    } else {
        resource.priorModified = res.modified;
        resource.priorExpires = res.expires;
        resource.priorEtag = res.etag;
        tile.setMetadata(res.modified, res.expires);
        tile.setData(res.noContent ? nullptr : res.data);
    }
}

template <typename T>
void TileLoader<T>::loadFromNetwork() {
    assert(!request);

    // Instead of using Resource::LoadingMethod::All, we're first doing a CacheOnly, and then a
    // NetworkOnly request.
    resource.loadingMethod = Resource::LoadingMethod::NetworkOnly;
    request = fileSource.request(resource, [this](Response res) { loadedData(res); });
}

} // namespace mbgl
