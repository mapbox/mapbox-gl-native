#ifndef MBGL_GEOMETRY_LINE_ATLAS
#define MBGL_GEOMETRY_LINE_ATLAS

#include <vector>
#include <map>
#include <mutex>
#include <atomic>

namespace mbgl {

typedef struct {
    float width;
    float height;
    float y;
} LinePatternPos;

class LineAtlas {
public:
    LineAtlas(uint16_t width, uint16_t height);
    ~LineAtlas();

    void bind();

    LinePatternPos getDashPosition(const std::vector<float>&, bool);
    LinePatternPos addDash(const std::vector<float> &dasharray, bool round);

    const int width;
    const int height;

private:
    std::recursive_mutex mtx;
    char *const data = nullptr;
    std::atomic<bool> dirty;
    uint32_t texture = 0;
    int nextRow = 0;
    std::map<size_t, LinePatternPos> positions;
};

};

#endif
