#include <llmr/geometry/elements_buffer.hpp>

using namespace llmr;

void TriangleElementsBuffer::add(element_type a, element_type b, element_type c) {
    element_type *elements = static_cast<element_type *>(addElement());
    elements[0] = a;
    elements[1] = b;
    elements[2] = c;
}

void LineElementsBuffer::add(element_type a, element_type b) {
    element_type *elements = static_cast<element_type *>(addElement());
    elements[0] = a;
    elements[1] = b;
}

void PointElementsBuffer::add(element_type a) {
    uint16_t *data = static_cast<element_type *>(addElement());
    data[0] = a;
}
