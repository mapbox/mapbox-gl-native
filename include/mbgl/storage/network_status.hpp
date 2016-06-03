#pragma once

#include <mbgl/util/atomic.hpp>

#include <mutex>
#include <set>

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
    static util::Atomic<bool> online;
    static std::mutex mtx;
    static std::set<util::AsyncTask*> observers;
};

} // namespace mbgl
