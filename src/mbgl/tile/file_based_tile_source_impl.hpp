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
    // The first request is always optional.
    resource.necessity = Resource::Optional;
    request = fileSource.request(resource, [this](Response res) {
        request.reset();
        loadedData(res);
        T::loaded = true;
        if (T::isRequired()) {
            loadRequired();
        }
    });
}

template <typename T, typename I>
void FileBasedTileSource<T, I>::makeRequired() {
    if (T::loaded && !request) {
        loadRequired();
    }
}

template <typename T, typename I>
void FileBasedTileSource<T, I>::makeOptional() {
    if (T::loaded && request) {
        // Abort a potential HTTP request.
        request.reset();
    }
}

template <typename T, typename I>
void FileBasedTileSource<T, I>::loadedData(const Response& res) {
    if (res.error && !T::loaded && res.error->reason == Response::Error::Reason::NotFound) {
        // When the optional request could not be satisfied, don't treat it as an error. Instead,
        // we make sure that the next request knows that there has been an optional request before
        // by setting one of the prior* fields.
        resource.priorExpires = Timestamp{ Seconds::zero() };
    } else if (res.error) {
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
    request = fileSource.request(resource, [this](Response res) {
        loadedData(res);
    });
}

} // namespace mbgl
