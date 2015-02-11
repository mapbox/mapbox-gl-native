#ifndef MBGL_STORAGE_NETWORK_STATUS
#define MBGL_STORAGE_NETWORK_STATUS

#include <mutex>
#include <set>

typedef struct uv_async_s uv_async_t;

namespace mbgl {

class NetworkStatus {
public:
    static void Reachable();

    static void Subscribe(uv_async_t *async);
    static void Unsubscribe(uv_async_t *async);

private:
    static std::mutex mtx;
    static std::set<uv_async_t *> observers;
};

}

#endif