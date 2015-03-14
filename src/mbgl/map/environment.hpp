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

enum class ThreadType : uint8_t {
    Unknown    = 0,
    Main       = 1 << 0,
    Map        = 1 << 1,
    TileWorker = 1 << 2,
};

class Environment final : private util::noncopyable {
public:
    class Scope final {
    public:
        Scope(Environment&, ThreadType, const std::string& name);
        ~Scope();

    private:
        std::thread::id id;
    };

    Environment(FileSource&);

    static Environment& Get();
    static bool inScope();
    static bool currentlyOn(ThreadType);
    static std::string threadName();

    unsigned getID() const;
    void requestAsync(const Resource&, std::function<void(const Response&)>);
    Request* request(const Resource&, std::function<void(const Response&)>);
    void cancelRequest(Request*);

    // Request to terminate the environment.
    void terminate();

private:
    unsigned id;
    FileSource& fileSource;

public:
    uv_loop_t* const loop;
};

}

#endif
