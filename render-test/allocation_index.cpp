#include "allocation_index.hpp"

#include <atomic>
#include <cassert>
#include <cstdlib>
#include <mutex>
#include <unordered_map>

namespace {

std::atomic_size_t indexedMemorySize{0};
std::atomic_size_t indexedMemoryPeak{0};
std::atomic_size_t allocationsCount{0};
std::unordered_map<void*, size_t> memoryIndex;
std::atomic_bool suppresIndexing{false};
std::atomic_bool active{false};
std::mutex indexMutex;

class FlagGuard {
public:
    explicit FlagGuard(std::atomic_bool& flag_)
        : flag(flag_) { flag = true; }
    ~FlagGuard() { flag = false; }

private:
    std::atomic_bool& flag;
};

void addToIndex(std::size_t sz, void* ptr) {
    std::lock_guard<std::mutex> mlk(indexMutex);
    FlagGuard flk(suppresIndexing);
    allocationsCount++;
    indexedMemorySize += sz;
    if (indexedMemoryPeak < indexedMemorySize) indexedMemoryPeak = size_t(indexedMemorySize);
    memoryIndex[ptr] = sz;
}

void removeFromIndex(void* ptr) {
    std::lock_guard<std::mutex> mlk(indexMutex);
    FlagGuard flk(suppresIndexing);
    auto it = memoryIndex.find(ptr);
    if (it == memoryIndex.end()) return;

    assert(indexedMemorySize >= it->second);
    indexedMemorySize -= it->second;
    memoryIndex.erase(it);
}

inline bool canModifyIndex() {
    return active && !suppresIndexing;
}

} // namespace

// static
void AllocationIndex::setActive(bool active_) {
    active = active_;
}

// static
bool AllocationIndex::isActive() {
    return active;
}

// static
void AllocationIndex::reset() {
    std::lock_guard<std::mutex> mlk(indexMutex);
    FlagGuard flk(suppresIndexing);
    memoryIndex.clear();
    indexedMemorySize = 0;
    allocationsCount = 0;
    indexedMemoryPeak = 0;
}

// static
void* AllocationIndex::allocate(size_t size) {
    void *ptr = std::malloc(size);
    if (ptr && canModifyIndex()) addToIndex(size, ptr);
    return ptr;
}

// static
void AllocationIndex::deallocate(void* ptr) noexcept {
    if (canModifyIndex()) removeFromIndex(ptr);
    std::free(ptr);
}

// static
size_t AllocationIndex::getAllocatedSize() {
    return indexedMemorySize;
}

// static
size_t AllocationIndex::getAllocationsCount() {
    return allocationsCount;
}

// static
size_t AllocationIndex::getAllocatedSizePeak() {
    return indexedMemoryPeak;
}
