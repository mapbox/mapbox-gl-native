#include "map_data.hpp"

#include <algorithm>

namespace mbgl {

// Adds the class if it's not yet set. Returns true when it added the class, and false when it
// was already present.
bool MapData::addClass(const std::string& klass) {
    Lock lock(mtx);
    if (std::find(classes.begin(), classes.end(), klass) != classes.end()) return false;
    classes.push_back(klass);
    return true;
}

// Removes the class if it's present. Returns true when it remvoed the class, and false when it
// was not present.
bool MapData::removeClass(const std::string& klass) {
    Lock lock(mtx);
    const auto it = std::find(classes.begin(), classes.end(), klass);
    if (it != classes.end()) {
        classes.erase(it);
        return true;
    }
    return false;
}

// Returns true when class is present in the list of currently set classes.
bool MapData::hasClass(const std::string& klass) const {
    Lock lock(mtx);
    return std::find(classes.begin(), classes.end(), klass) != classes.end();
}

void MapData::setClasses(const std::vector<std::string>& klasses) {
    Lock lock(mtx);
    classes = klasses;
}

std::vector<std::string> MapData::getClasses() const {
    Lock lock(mtx);
    return classes;
}

}
