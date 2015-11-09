#ifndef MBGL_STORAGE_FRONTLINE_FILE_SOURCE
#define MBGL_STORAGE_FRONTLINE_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>

namespace mbgl {

namespace util {
template <typename T> class Thread;
}

class FrontlineFileSource : public FileSource {
public:
    FrontlineFileSource(const std::string& path);
    ~FrontlineFileSource() override;

    bool handlesResource(const Resource&) override;
    Request* request(const Resource&, uv_loop_t*, Callback) override;
    void cancel(Request*) override;

public:
    class Impl;
private:
    const std::unique_ptr<util::Thread<Impl>> thread;
};

}

#endif
