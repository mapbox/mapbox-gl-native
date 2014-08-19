#ifndef MBGL_GEOMETRY_LINE_ATLAS
#define MBGL_GEOMETRY_LINE_ATLAS

#include <vector>
#include <map>

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
    void addDash(std::vector<float> &dasharray, bool round);
    LinePatternPos getPattern(const std::string &name);

    const uint32_t width = 0;
    const uint32_t height = 0;

private:
    char *const data = nullptr;
    std::atomic<bool> dirty;
    uint32_t texture = 0;
    uint32_t nextRow = 0;
    std::map<std::string, LinePatternPos> positions;
};

};

#endif
