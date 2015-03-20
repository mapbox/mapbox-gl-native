#ifndef MBGL_MAP_MAP_DATA
#define MBGL_MAP_MAP_DATA

#include <string>
#include <mutex>

namespace mbgl {

struct StyleInfo {
    std::string url;
    std::string base;
    std::string json;
};

class MapData {
    using Lock = std::lock_guard<std::mutex>;

public:
    inline StyleInfo getStyleInfo() const {
        Lock lock(mtx);
        return styleInfo;
    }
    inline void setStyleInfo(StyleInfo&& info) {
        Lock lock(mtx);
        styleInfo = info;
    }

private:
    mutable std::mutex mtx;

    StyleInfo styleInfo;
};

}

#endif
