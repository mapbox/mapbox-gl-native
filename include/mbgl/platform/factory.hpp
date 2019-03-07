#pragma once

#include <mbgl/storage/file_source_options.hpp>

#include <memory>

namespace mbgl {

class FileSource;

namespace platform {

class Factory {
public:
    // This is also a singleton: it stores a weak reference to the managed object once it has been created upon first
    // call of this function. It allows the initial value to be overriden.
    static std::shared_ptr<FileSource> sharedFileSource(const FileSourceOptions& options = {}, std::shared_ptr<FileSource> initial = {});
};

} // namespace platform
} // namespace mbgl
