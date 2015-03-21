#include <mbgl/map/environment.hpp>

#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/async_queue.hpp>

#include <uv.h>

#include <atomic>
#include <cassert>
#include <mutex>
#include <unordered_map>

namespace mbgl {

namespace {

class ThreadInfoStore {
private:
    struct ThreadInfo {
        Environment* env;
        ThreadType type;
        std::string name;
        uv_loop_t* loop;
        util::AsyncQueue<Closure>* queue;
    };

public:
    ThreadInfoStore() {
        registerThread(nullptr, ThreadType::Main, "Main", nullptr);
    }

    ~ThreadInfoStore() {
        unregisterThread();
        assert(threadSet.size() == 0);
    }

    void registerThread(Environment* env, ThreadType type, const std::string& name, uv_loop_t* loop) {
        std::lock_guard<std::recursive_mutex> lock(mtx);

        util::AsyncQueue<Closure>* queue(nullptr);
        if (loop) {
            queue = new util::AsyncQueue<Closure>(loop, postTaskCallback);
            queue->unref();
        }

        // FIXME: We should never need to overwrite a thread here and we only allow
        // this today because on the Static mode, the Map thread and the Main thread
        // are same. Replace this with emplace() when this gets fixed.
        threadSet[std::this_thread::get_id()] = ThreadInfo{env, type, name, loop, queue};
    }

    void unregisterThread() {
        std::lock_guard<std::recursive_mutex> lock(mtx);

        ThreadSet::iterator it = threadSet.find(std::this_thread::get_id());
        if (it == threadSet.end()) {
            return;
        }

        if (it->second.queue) {
            it->second.queue->stop();
        }

        threadSet.erase(it);
    }

    const ThreadInfo& getThreadInfo(std::thread::id id = std::this_thread::get_id()) const {
        static ThreadInfo emptyInfo;
        std::lock_guard<std::recursive_mutex> lock(mtx);

        ThreadSet::const_iterator it = threadSet.find(id);
        if (it != threadSet.end()) {
            return it->second;
        } else {
            return emptyInfo;
        }
    }

    bool postTask(const std::thread::id& to, const Closure& task) {
        std::lock_guard<std::recursive_mutex> lock(mtx);

        util::AsyncQueue<Closure>* queue = getThreadInfo(to).queue;
        if (!queue) {
            Log::Debug(mbgl::Event::Thread, "Queue not available, discarding task");
            return false;
        }

        queue->send(Closure(task));
        return true;
    }

private:
    static void postTaskCallback(Closure& task) {
        task();
    }

    typedef std::unordered_map<std::thread::id, ThreadInfo> ThreadSet;
    ThreadSet threadSet;

    mutable std::recursive_mutex mtx;
};

uint32_t makeEnvironmentID() {
    static std::atomic<uint32_t> id(0);
    return id++;
}

ThreadInfoStore threadInfoStore;

} // namespace

EnvironmentScope::EnvironmentScope(Environment& env,
                                   ThreadType type,
                                   const std::string& name,
                                   uv_loop_t* loop)
    : tid(std::this_thread::get_id()) {
    threadInfoStore.registerThread(&env, type, name, loop);
}

EnvironmentScope::~EnvironmentScope() {
    assert(tid == std::this_thread::get_id());
    threadInfoStore.unregisterThread();
}

Environment::Environment(FileSource& fs)
    : id(makeEnvironmentID()), fileSource(fs), loop(uv_loop_new()) {
}

Environment::~Environment() {
    assert(abandonedVAOs.empty());
    assert(abandonedTextures.empty());
    assert(abandonedBuffers.empty());
}

Environment& Environment::Get() {
    Environment* env = threadInfoStore.getThreadInfo().env;
    assert(env);

    return *env;
}

bool Environment::inScope() {
    return threadInfoStore.getThreadInfo().env;
}

bool Environment::currentlyOn(ThreadType type) {
    return static_cast<uint8_t>(threadInfoStore.getThreadInfo().type) & static_cast<uint8_t>(type);
}

std::string Environment::threadName() {
    return threadInfoStore.getThreadInfo().name;
}

bool Environment::postTask(const std::thread::id& to, const Closure& task) {
    return threadInfoStore.postTask(to, task);
}

uint32_t Environment::getID() const {
    return id;
}

void Environment::requestAsync(const Resource& resource,
                               std::function<void(const Response&)> callback) {
    fileSource.request(resource, std::move(callback));
}

Request* Environment::request(const Resource& resource,
                              std::function<void(const Response&)> callback) {
    assert(currentlyOn(ThreadType::Map));
    return fileSource.request(resource, loop, std::move(callback));
}

void Environment::cancelRequest(Request* req) {
    assert(currentlyOn(ThreadType::Map));
    fileSource.cancel(req);
}

// #############################################################################################

#pragma mark - OpenGL cleanup

void Environment::abandonVAO(uint32_t vao) {
    assert(currentlyOn(ThreadType::Map));
    abandonedVAOs.emplace_back(vao);
}

void Environment::abandonBuffer(uint32_t buffer) {
    assert(currentlyOn(ThreadType::Map));
    abandonedBuffers.emplace_back(buffer);
}

void Environment::abandonTexture(uint32_t texture) {
    assert(currentlyOn(ThreadType::Map));
    abandonedTextures.emplace_back(texture);
}

// Actually remove the objects we marked as abandoned with the above methods.
void Environment::performCleanup() {
    assert(currentlyOn(ThreadType::Map));

    if (!abandonedVAOs.empty()) {
        MBGL_CHECK_ERROR(gl::DeleteVertexArrays(static_cast<GLsizei>(abandonedVAOs.size()),
                                                abandonedVAOs.data()));
        abandonedVAOs.clear();
    }

    if (!abandonedTextures.empty()) {
        MBGL_CHECK_ERROR(glDeleteTextures(static_cast<GLsizei>(abandonedTextures.size()),
                                          abandonedTextures.data()));
        abandonedTextures.clear();
    }

    if (!abandonedBuffers.empty()) {
        MBGL_CHECK_ERROR(glDeleteBuffers(static_cast<GLsizei>(abandonedBuffers.size()),
                                         abandonedBuffers.data()));
        abandonedBuffers.clear();
    }
}

// #############################################################################################

void Environment::terminate() {
    fileSource.abort(id);
}

}
