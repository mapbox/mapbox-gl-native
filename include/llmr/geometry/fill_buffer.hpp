#ifndef LLMR_GEOMETRY_FILL_BUFFER
#define LLMR_GEOMETRY_FILL_BUFFER

#include <vector>
#include <cstdint>

namespace llmr {

class FillBuffer {
public:
    typedef int16_t vertex_type;
    const uint8_t components = 2;

    FillBuffer();
    ~FillBuffer();

    uint32_t vertex_length() const;
    uint32_t elements_length() const;

    void addDegenerate();
    void addCoordinate(vertex_type x, vertex_type y);
    void addElements(uint16_t a, uint16_t b, uint16_t c);

    void bind();

private:
    std::vector<vertex_type> vertices;
    std::vector<uint16_t> elements;
    uint32_t vertex_buffer;
    uint32_t element_buffer;
    bool dirty;
};

}

#endif
