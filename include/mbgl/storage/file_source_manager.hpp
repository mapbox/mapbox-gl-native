#pragma once

#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/pass_types.hpp>

namespace mbgl {

class ResourceOptions;

/**
 * @brief A singleton class responsible for managing file sources.
 *
 * The FileSourceManager provides following functionality:
 *
 * - provides access to file sources of a specific type and configuration
 * - caches previously created file sources of a (type, configuration) tuples
 * - allows to register and unregister file source factories
 */
class FileSourceManager {
public:
    using FileSourceFactory = std::function<std::unique_ptr<FileSource>(const ResourceOptions&)>;

    /**
     * @brief A singleton getter.
     *
     * @return FileSourceManager*
     */
    static FileSourceManager* get() noexcept;

    // Returns shared instance of a file source for (type, options) tuple.
    // Creates new instance via registered factory if needed. If new instance cannot be
    // created, nullptr would be returned.
    PassRefPtr<FileSource> getFileSource(FileSourceType, const ResourceOptions&) noexcept;

    // Registers file source factory for a provided FileSourceType type. If factory for the
    // same type was already registered, will unregister previously registered factory.
    // Provided factory must not be null.
    virtual void registerFileSourceFactory(FileSourceType, FileSourceFactory&&) noexcept;

    // Unregisters file source factory. If there are no registered factories for a FileSourceType
    // invocation has no effect.
    virtual FileSourceFactory unRegisterFileSourceFactory(FileSourceType) noexcept;

protected:
    FileSourceManager();
    class Impl;
    std::unique_ptr<Impl> impl;
    virtual ~FileSourceManager();
};

} // namespace mbgl
