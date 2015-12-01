#ifndef MBGL_STORAGE_NETWORK_STATUS
#define MBGL_STORAGE_NETWORK_STATUS

#include <mutex>
#include <set>

namespace mbgl {

namespace util {
class AsyncTask;
} // namespace util

class NetworkStatus {
public:
    static void Reachable();

    static void Subscribe(util::AsyncTask* async);
    static void Unsubscribe(util::AsyncTask* async);

private:
    static std::mutex mtx;
    static std::set<util::AsyncTask*> observers;
};

} // namespace mbgl

#endif
