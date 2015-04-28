#ifndef MBGL_MAP_MAP_ENVIRONMENT
#define MBGL_MAP_MAP_ENVIRONMENT

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/util.hpp>

#include <thread>
#include <functional>
#include <vector>

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
    Environment(FileSource&);
    ~Environment();

    static Environment& Get();
    static bool inScope();
    static bool currentlyOn(ThreadType);
    static std::string threadName();

    unsigned getID() const;

    // #############################################################################################

    // File request APIs
    void requestAsync(const Resource&, std::function<void(const Response&)>);
    Request* request(const Resource&, std::function<void(const Response&)>);
    void cancelRequest(Request*);

    // #############################################################################################

    // Mark OpenGL objects for deletion
    void abandonVAO(uint32_t vao);
    void abandonBuffer(uint32_t buffer);
    void abandonTexture(uint32_t texture);

    // Actually remove the objects we marked as abandoned with the above methods.
    // Only call this while the OpenGL context is exclusive to this thread.
    void performCleanup();

private:
    unsigned id;
    FileSource& fileSource;

    // Stores OpenGL objects that we marked for deletion
    std::vector<uint32_t> abandonedVAOs;
    std::vector<uint32_t> abandonedBuffers;
    std::vector<uint32_t> abandonedTextures;

public:
    uv_loop_t* const loop;
};

class EnvironmentScope final {
public:
    EnvironmentScope(Environment&, ThreadType, const std::string& name);
    ~EnvironmentScope();

private:
    std::thread::id id;
};

}

#endif
