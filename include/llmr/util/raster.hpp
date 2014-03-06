#ifndef LLMR_UTIL_RASTER
#define LLMR_UTIL_RASTER

#include <string>
#include <mutex>

namespace llmr {
namespace util {

class Raster : public std::enable_shared_from_this<Raster> {

public:
    ~Raster();

    void load(const std::string& data);
    void bind(bool linear = false);

    operator bool() const;

public:
    uint32_t width = 0, height = 0;

private:
    void loadImage(const std::string& data);

private:
    mutable std::mutex mtx;
    bool loaded = false;
    uint32_t filter = 0;
    uint32_t texture = 0;
    char *img = nullptr;
};

}
}

#endif
