#include <mbgl/storage/network_status.hpp>

#include <mbgl/util/async_task.hpp>

// Example: Allocate a reachability object
// Reachability* reach = [Reachability reachabilityForInternetConnection];
// reach.reachableBlock = ^(Reachability* reach) { NetworkStatus::Reachable(); };
// [reach startNotifier];

namespace mbgl {

std::mutex NetworkStatus::mtx;
std::set<util::AsyncTask *> NetworkStatus::observers;

void NetworkStatus::Subscribe(util::AsyncTask *async) {
    std::lock_guard<std::mutex> lock(NetworkStatus::mtx);
    observers.insert(async);
}

void NetworkStatus::Unsubscribe(util::AsyncTask *async) {
    std::lock_guard<std::mutex> lock(NetworkStatus::mtx);
    observers.erase(async);
}

void NetworkStatus::Reachable() {
    std::lock_guard<std::mutex> lock(NetworkStatus::mtx);
    for (auto async : observers) {
        async->send();
    }
}

}
