#ifndef MBGL_GEOMETRY_LINE_ATLAS
#define MBGL_GEOMETRY_LINE_ATLAS

#include <vector>
#include <map>
#include <memory>
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

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    void bind();

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload();

    LinePatternPos getDashPosition(const std::vector<float>&, bool);
    LinePatternPos addDash(const std::vector<float> &dasharray, bool round);

    const int width;
    const int height;

private:
    std::recursive_mutex mtx;
    const std::unique_ptr<uint8_t[]> data;
    std::atomic<bool> dirty;
    uint32_t texture = 0;
    int nextRow = 0;
    std::map<size_t, LinePatternPos> positions;
};

};

#endif
