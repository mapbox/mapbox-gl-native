#include <mbgl/util/id.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wredeclared-class-member"
#include <boost/bimap.hpp>
#pragma clang diagnostic pop

#include <mutex>

namespace mbgl {
namespace util {

namespace {

std::mutex mutex;
boost::bimap<std::string, Dictionary::Type> store;
Dictionary::Type counter = 1;
const std::string empty = "";

} // namespace

Dictionary::Type Dictionary::Lookup(const std::string& name) {
    if (name.empty()) {
        return Type();
    } else {
        std::lock_guard<std::mutex> lock(mutex);
        const auto it = store.left.find(name);
        if (it == store.left.end()) {
            return store.insert({ name, counter++ }).first->right;
        } else {
            return it->second;
        }
    }
}

const std::string& Dictionary::Lookup(Type id) {
    if (!id) {
        return empty;
    }

    std::lock_guard<std::mutex> lock(mutex);
    const auto it = store.right.find(id);
    if (it == store.right.end()) {
        return empty;
    } else {
        return it->second;
    }
}

} // namespace util
} // namespace mbgl
