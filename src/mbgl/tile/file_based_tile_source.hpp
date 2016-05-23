#pragma once

#include <mbgl/storage/resource.hpp>

namespace mbgl {

class FileSource;
class AsyncRequest;

template <typename T, typename I>
class FileBasedTileSource : public T {
protected:
    FileBasedTileSource(typename T::data_type&, const Resource&, FileSource&);
    virtual ~FileBasedTileSource() = default;

protected:
    const Resource resource;
    FileSource& fileSource;
    std::unique_ptr<AsyncRequest> request;
};

} // namespace mbgl
