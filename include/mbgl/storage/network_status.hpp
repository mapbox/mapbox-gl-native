#ifndef MBGL_STORAGE_NETWORK_STATUS
#define MBGL_STORAGE_NETWORK_STATUS

#include <atomic>
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

    Status Get();
    void Set(Status);

    static void Reachable();

    static void Subscribe(util::AsyncTask* async);
    static void Unsubscribe(util::AsyncTask* async);

private:
    static std::atomic<bool> online;
    static std::mutex mtx;
    static std::set<util::AsyncTask*> observers;
};

} // namespace mbgl

#endif
