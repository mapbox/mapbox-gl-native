#ifndef MBGL_STORAGE_OFFLINE_FILE_SOURCE
#define MBGL_STORAGE_OFFLINE_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/work_request.hpp>

namespace mbgl {

namespace util {
template <typename T> class Thread;
} // namespace util

class OfflineFileSource : public FileSource {
public:
    OfflineFileSource(const std::string& path);
    ~OfflineFileSource() override;

    std::unique_ptr<FileRequest> request(const Resource&, Callback) override;

public:
    friend class FrontlineFileRequest;

    class Impl;
    const std::unique_ptr<util::Thread<Impl>> thread;
};

} // namespace mbgl

#endif
