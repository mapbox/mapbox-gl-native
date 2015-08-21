#include <mbgl/storage/network_status.hpp>

#include <uv.h>

// Example: Allocate a reachability object
// Reachability* reach = [Reachability reachabilityForInternetConnection];
// reach.reachableBlock = ^(Reachability* reach) { NetworkStatus::Reachable(); };
// [reach startNotifier];

namespace mbgl {

std::mutex NetworkStatus::mtx;
std::set<uv_async_t *> NetworkStatus::observers;

void NetworkStatus::Subscribe(uv_async_t *async) {
    std::lock_guard<std::mutex> lock(NetworkStatus::mtx);
    observers.insert(async);
}

void NetworkStatus::Unsubscribe(uv_async_t *async) {
    std::lock_guard<std::mutex> lock(NetworkStatus::mtx);
    observers.erase(async);
}

void NetworkStatus::Reachable() {
    std::lock_guard<std::mutex> lock(NetworkStatus::mtx);
    for (auto async : observers) {
        uv_async_send(async);
    }
}

}
