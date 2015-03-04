#ifndef MBGL_MAP_MAP_ENVIRONMENT
#define MBGL_MAP_MAP_ENVIRONMENT

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/util.hpp>

#include <thread>
#include <functional>

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class FileSource;
class Request;
class Response;
struct Resource;

class Environment : private util::noncopyable {
public:
    Environment(FileSource &);

    void setup();

    bool inMapThread() const;

    void requestAsync(const Resource &, std::function<void(const Response &)>);
    Request *request(const Resource &, std::function<void(const Response &)>);
    void cancelRequest(Request *);

    // Request to terminate the environment.
    void terminate();

private:
    FileSource &fileSource;
    std::thread::id mapThread;

public:
    uv_loop_t *const loop;
};

}

#endif
