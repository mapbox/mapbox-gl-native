#ifndef MBGL_MAP_MAP_DATA
#define MBGL_MAP_MAP_DATA

#include <string>
#include <mutex>
#include <atomic>

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

    inline bool getDebug() const { return debug; }
    inline bool toggleDebug() { return debug ^= 1u; }
    inline void setDebug(bool value) { debug = value; }

private:
    mutable std::mutex mtx;

    StyleInfo styleInfo;
    std::atomic<uint8_t> debug;
};

}

#endif
