#ifndef MBGL_GEOMETRY_LINE_ATLAS
#define MBGL_GEOMETRY_LINE_ATLAS

#include <vector>

#include <atomic>

namespace mbgl {

class LineAtlas {
public:
    LineAtlas(uint16_t width, uint16_t height);
    ~LineAtlas();

    void bind();
    void addDash(std::vector<float> &dasharray, bool round);

    const uint32_t width = 0;
    const uint32_t height = 0;

private:
    char *const data = nullptr;
    std::atomic<bool> dirty;
    uint32_t texture = 0;
};
};

#endif
