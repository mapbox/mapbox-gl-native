#ifndef MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE
#define MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/file_cache.hpp>
#include <mbgl/util/run_loop.hpp>

namespace mbgl {

class DefaultFileSource : public FileSource {
public:
    DefaultFileSource(FileCache *cache, const std::string &root = "");
    ~DefaultFileSource() override;

    // FileSource API
    Request *request(const Resource &resource, uv_loop_t *loop, const Environment &env,
                     Callback callback) override;
    void cancel(Request *request) override;
    void request(const Resource &resource, const Environment &env, Callback callback) override;

    void abort(const Environment &env) override;

public:
    class Impl;
private:
    const std::unique_ptr<util::Thread<Impl>> thread;
};

}

#endif
