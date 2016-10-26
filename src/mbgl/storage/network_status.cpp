#include <mbgl/storage/network_status.hpp>

#include <mbgl/util/async_task.hpp>

// Example: Allocate a reachability object
// Reachability* reach = [Reachability reachabilityForInternetConnection];
// reach.reachableBlock = ^(Reachability* reach) { NetworkStatus::Reachable(); };
// [reach startNotifier];

namespace mbgl {

std::atomic<bool> NetworkStatus::online(true);
std::mutex NetworkStatus::mtx;
std::unordered_set<util::AsyncTask *> NetworkStatus::observers;

NetworkStatus::Status NetworkStatus::Get() {
    if (online) {
        return Status::Online;
    } else {
        return Status::Offline;
    }
}

void NetworkStatus::Set(Status status) {
    if (status == Status::Offline) {
        online = false;
    } else if(!online) {
        online = true;
        Reachable();
    }
}

void NetworkStatus::Subscribe(util::AsyncTask *async) {
    std::lock_guard<std::mutex> lock(NetworkStatus::mtx);
    observers.insert(async);
}

void NetworkStatus::Unsubscribe(util::AsyncTask *async) {
    std::lock_guard<std::mutex> lock(NetworkStatus::mtx);
    observers.erase(async);
}

void NetworkStatus::Reachable() {
    if (!online) {
        return;
    }

    std::lock_guard<std::mutex> lock(NetworkStatus::mtx);
    for (auto async : observers) {
        async->send();
    }
}

} // namespace mbgl
