#pragma once

#include <mbgl/tile/file_based_tile_source.hpp>
#include <mbgl/storage/file_source.hpp>

namespace mbgl {

template <typename T, typename I>
FileBasedTileSource<T, I>::FileBasedTileSource(typename T::data_type& tileData_,
                                               const Resource& resource_,
                                               FileSource& fileSource_)
    : T(tileData_), resource(resource_), fileSource(fileSource_) {
    request = fileSource.request(resource, [this](Response res) {
        if (res.error) {
            T::tileData.setError(std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            T::tileData.setData(nullptr, res.modified, res.expires);
        } else {

            T::tileData.setData(I::parseData(res.data), res.modified, res.expires);
        }
    });
}

} // namespace mbgl
