#pragma once

#include <mbgl/tile/file_based_tile_source.hpp>
#include <mbgl/storage/file_source.hpp>

#include <cassert>

namespace mbgl {

template <typename T, typename I>
FileBasedTileSource<T, I>::FileBasedTileSource(typename T::data_type& tileData_,
                                               const Resource& resource_,
                                               FileSource& fileSource_)
    : T(tileData_), resource(resource_), fileSource(fileSource_) {
    assert(!request);
    if (fileSource.supportsOptionalRequests()) {
        // When supported, the first request is always optional, even if the TileSource
        // is marked as required. That way, we can let the first optional request continue
        // to load when the TileSource is later changed from required to optional. If we
        // started out with a required request, we'd have to cancel everything, including the
        // initial optional part of the request.
        loadOptional();
    } else {
        // When the FileSource doesn't support optional requests, we do nothing until the
        // data is definitely required.
        if (T::isRequired()) {
            loadRequired();
        } else {
            // We're using this field to check whether the pending request is optional or required.
            resource.necessity = Resource::Optional;
        }
    }
}

template <typename T, typename I>
void FileBasedTileSource<T, I>::loadOptional() {
    assert(!request);

    resource.necessity = Resource::Optional;
    request = fileSource.request(resource, [this](Response res) {
        request.reset();

        if (res.error && res.error->reason == Response::Error::Reason::NotFound) {
            // When the optional request could not be satisfied, don't treat it as an error.
            // Instead, we make sure that the next request knows that there has been an optional
            // request before by setting one of the prior* fields.
            resource.priorExpires = Timestamp{ Seconds::zero() };
        } else {
            loadedData(res);
        }

        if (T::isRequired()) {
            loadRequired();
        }
    });
}

template <typename T, typename I>
void FileBasedTileSource<T, I>::makeRequired() {
    if (!request) {
        loadRequired();
    }
}

template <typename T, typename I>
void FileBasedTileSource<T, I>::makeOptional() {
    if (resource.necessity == Resource::Required && request) {
        // Abort a potential HTTP request.
        request.reset();
    }
}

template <typename T, typename I>
void FileBasedTileSource<T, I>::loadedData(const Response& res) {
    if (res.error && res.error->reason != Response::Error::Reason::NotFound) {
        T::tileData.setError(std::make_exception_ptr(std::runtime_error(res.error->message)));
    } else if (res.notModified) {
        resource.priorExpires = res.expires;
        // Do not notify the TileData object; when we get this message, it already has the current
        // version of the data.
    } else {
        resource.priorModified = res.modified;
        resource.priorExpires = res.expires;
        resource.priorEtag = res.etag;
        T::tileData.setData(res.noContent ? nullptr : I::parseData(res.data), res.modified,
                            res.expires);
    }
}

template <typename T, typename I>
void FileBasedTileSource<T, I>::loadRequired() {
    assert(!request);

    resource.necessity = Resource::Required;
    request = fileSource.request(resource, [this](Response res) { loadedData(res); });
}

} // namespace mbgl
