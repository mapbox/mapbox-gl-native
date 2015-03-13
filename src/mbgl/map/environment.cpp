#include <mbgl/map/environment.hpp>
#include <mbgl/storage/file_source.hpp>

#include <uv.h>

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

        // FIXME: We should never need to overwrite a thread here and we only allow
        // this today because on the Static mode, the Map thread and the Main thread
        // are same. Replace this with emplace() when this gets fixed.
        threadSet[std::this_thread::get_id()] = ThreadInfo{env, type, name};
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

ThreadInfoStore threadInfoStore;

} // namespace

Environment::Scope::Scope(Environment& env, ThreadType type, const std::string& name)
    : id(std::this_thread::get_id()) {
    threadInfoStore.registerThread(&env, type, name);
}

Environment::Scope::~Scope() {
    assert(id == std::this_thread::get_id());
    threadInfoStore.unregisterThread();
}

Environment::Environment(FileSource &fs) : fileSource(fs), loop(uv_loop_new()) {
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

void Environment::requestAsync(const Resource &resource, std::function<void(const Response &)> callback) {
    fileSource.request(resource, *this, std::move(callback));
}

Request *Environment::request(const Resource &resource, std::function<void(const Response &)> callback) {
    assert(currentlyOn(ThreadType::Map));
    return fileSource.request(resource, loop, *this, std::move(callback));
}

void Environment::cancelRequest(Request *req) {
    assert(currentlyOn(ThreadType::Map));
    fileSource.cancel(req);
}

void Environment::terminate() {
    fileSource.abort(*this);
}

}
