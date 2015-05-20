#include <mbgl/map/environment.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/geometry/vao.hpp>

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
    };

public:
    ThreadInfoStore() {
        registerThread(nullptr, ThreadType::Main, "Main");
    }

    ~ThreadInfoStore() {
        unregisterThread();
        assert(threadSet.size() == 0);
    }

    void registerThread(Environment* env, ThreadType type, const std::string& name) {
        std::lock_guard<std::mutex> lock(mtx);

        threadSet.emplace(std::this_thread::get_id(), ThreadInfo{ env, type, name });
    }

    void unregisterThread() {
        std::lock_guard<std::mutex> lock(mtx);

        ThreadSet::iterator it = threadSet.find(std::this_thread::get_id());
        if (it != threadSet.end()) {
            threadSet.erase(it);
        }
    }

    const ThreadInfo& getThreadInfo() const {
        static ThreadInfo emptyInfo;
        std::lock_guard<std::mutex> lock(mtx);

        ThreadSet::const_iterator it = threadSet.find(std::this_thread::get_id());
        if (it != threadSet.end()) {
            return it->second;
        } else {
            return emptyInfo;
        }
    }

private:
    typedef std::unordered_map<std::thread::id, ThreadInfo> ThreadSet;
    ThreadSet threadSet;

    mutable std::mutex mtx;
};

unsigned makeEnvironmentID() {
    static std::atomic<unsigned> id(0);
    return id++;
}

ThreadInfoStore threadInfoStore;

} // namespace

EnvironmentScope::EnvironmentScope(Environment& env, ThreadType type, const std::string& name)
    : id(std::this_thread::get_id()) {
    threadInfoStore.registerThread(&env, type, name);
}

EnvironmentScope::~EnvironmentScope() {
    assert(id == std::this_thread::get_id());
    threadInfoStore.unregisterThread();
}

Environment::Environment(FileSource& fs)
    : id(makeEnvironmentID()), fileSource(fs) {
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

unsigned Environment::getID() const {
    return id;
}

Request* Environment::request(const Resource& resource,
                              std::function<void(const Response&)> callback) {
    return fileSource.request(resource, util::RunLoop::current.get()->get(), std::move(callback));
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
        MBGL_CHECK_ERROR(VertexArrayObject::Delete(static_cast<GLsizei>(abandonedVAOs.size()),
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

}
