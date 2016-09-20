#pragma once

#include <atomic>
#include <mutex>
#include <unordered_set>

namespace mbgl {

namespace util {
class AsyncTask;
} // namespace util

class NetworkStatus {
public:
    enum class Status : uint8_t {
        Online,
        Offline,
    };

    static Status Get();
    static void Set(Status);

    static void Reachable();

    static void Subscribe(util::AsyncTask* async);
    static void Unsubscribe(util::AsyncTask* async);

private:
    static std::atomic<bool> online;
    static std::mutex mtx;
    static std::unordered_set<util::AsyncTask*> observers;
};

} // namespace mbgl
