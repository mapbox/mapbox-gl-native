#pragma once

#include <mapbox/geometry/wagyu/config.hpp>
#include <mapbox/geometry/wagyu/local_minimum.hpp>

#include <algorithm>

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
using scanbeam_list = std::vector<T>;

template <typename T>
bool pop_from_scanbeam(T& Y, scanbeam_list<T>& scanbeam) {
    if (scanbeam.empty()) {
        return false;
    }
    std::sort(scanbeam.begin(), scanbeam.end());
    scanbeam.erase(std::unique(scanbeam.begin(), scanbeam.end()), scanbeam.end());
    Y = scanbeam.back();
    scanbeam.pop_back();
    return true;
}

template <typename T>
void setup_scanbeam(local_minimum_list<T>& minima_list, scanbeam_list<T>& scanbeam) {

    for (auto lm = minima_list.begin(); lm != minima_list.end(); ++lm) {
        scanbeam.push_back(lm->y);
    }
}
}
}
}
