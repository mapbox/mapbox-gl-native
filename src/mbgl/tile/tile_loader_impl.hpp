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
      necessity(Necessity::Optional),
      resource(Resource::tile(
        tileset.tiles.at(0),
        parameters.pixelRatio,
        id.canonical.x,
        id.canonical.y,
        id.canonical.z,
        tileset.scheme)),
      fileSource(parameters.fileSource) {
    assert(!request);
    if (fileSource.supportsOptionalRequests()) {
        // When supported, the first request is always optional, even if the TileLoader
        // is marked as required. That way, we can let the first optional request continue
        // to load when the TileLoader is later changed from required to optional. If we
        // started out with a required request, we'd have to cancel everything, including the
        // initial optional part of the request.
        loadOptional();
    } else {
        // When the FileSource doesn't support optional requests, we do nothing until the
        // data is definitely required.
        if (necessity == Necessity::Required) {
            loadRequired();
        } else {
            // We're using this field to check whether the pending request is optional or required.
            resource.necessity = Resource::Optional;
        }
    }
}

template <typename T>
TileLoader<T>::~TileLoader() = default;

template <typename T>
void TileLoader<T>::loadOptional() {
    assert(!request);

    resource.necessity = Resource::Optional;
    request = fileSource.request(resource, [this](Response res) {
        request.reset();

        tile.setTriedOptional();

        if (res.error && res.error->reason == Response::Error::Reason::NotFound) {
            // When the optional request could not be satisfied, don't treat it as an error.
            // Instead, we make sure that the next request knows that there has been an optional
            // request before by setting one of the prior* fields.
            resource.priorExpires = Timestamp{ Seconds::zero() };
        } else {
            loadedData(res);
        }

        if (necessity == Necessity::Required) {
            loadRequired();
        }
    });
}

template <typename T>
void TileLoader<T>::makeRequired() {
    if (!request) {
        loadRequired();
    }
}

template <typename T>
void TileLoader<T>::makeOptional() {
    if (resource.necessity == Resource::Required && request) {
        // Abort a potential HTTP request.
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
    } else {
        resource.priorModified = res.modified;
        resource.priorExpires = res.expires;
        resource.priorEtag = res.etag;
        tile.setData(res.noContent ? nullptr : res.data, res.modified, res.expires);
    }
}

template <typename T>
void TileLoader<T>::loadRequired() {
    assert(!request);

    resource.necessity = Resource::Required;
    request = fileSource.request(resource, [this](Response res) { loadedData(res); });
}

} // namespace mbgl
