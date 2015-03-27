#ifndef MBGL_MAP_MAP_ENVIRONMENT
#define MBGL_MAP_MAP_ENVIRONMENT

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/util.hpp>

#include <future>
#include <thread>
#include <functional>
#include <vector>

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class FileSource;
class Request;
class Response;
struct Resource;

typedef std::function<void()> Closure;

enum class ThreadType : uint8_t {
    Unknown    = 0,
    Main       = 1 << 0,
    Map        = 1 << 1,
    TileWorker = 1 << 2,
    Test       = 1 << 3,
};

class Environment final : private util::noncopyable {
public:
    Environment(FileSource&);
    ~Environment();

    static Environment& Get();
    static bool inScope();
    static bool currentlyOn(ThreadType);
    static std::string threadName();

    static bool postTask(const std::thread::id& to, const Closure& task);

    template<typename T>
    static bool postTaskSync(const std::thread::id& to, const std::function<T()>& task, T& out) {
        std::promise<T> promise;

        auto syncTask = [&promise, task] {
             promise.set_value(task());
        };

        if (!postTask(to, syncTask)) {
            return false;
        }

        out = std::move(promise.get_future().get());
        return true;
    }

    uint32_t getID() const;

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

    // #############################################################################################

    // Request to terminate the environment.
    void terminate();

private:
    uint32_t id;
    FileSource& fileSource;

    // Stores OpenGL objects that we marked for deletion
    std::vector<uint32_t> abandonedVAOs;
    std::vector<uint32_t> abandonedBuffers;
    std::vector<uint32_t> abandonedTextures;
};

class EnvironmentScope final {
public:
    EnvironmentScope(Environment&, ThreadType, const std::string& name, uv_loop_t* loop = nullptr);
    ~EnvironmentScope();

private:
    std::thread::id tid;
};

}

#endif
