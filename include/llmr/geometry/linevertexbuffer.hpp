#ifndef LLMR_GEOMETRY_LINEVERTEXBUFFER
#define LLMR_GEOMETRY_LINEVERTEXBUFFER

#include <vector>

namespace llmr {

class linevertexbuffer {
public:
    linevertexbuffer();

    void addDegenerate();
    void addCoordinate(int16_t x, int16_t y);

    uint32_t length();
    void bind();
private:
    std::vector<uint16_t> array;
    uint32_t buffer;
};

}

#endif
