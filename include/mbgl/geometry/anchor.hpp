#ifndef MBGL_GEOMETRY_ANCHOR
#define MBGL_GEOMETRY_ANCHOR

#include <vector>

namespace mbgl {

struct Anchor {
    float x = 0.0f;
    float y = 0.0f;
    float angle = 0.0f;
    float scale = 0.0f;
    int segment = -1;

    explicit Anchor(float x, float y, float angle, float scale)
        : x(x), y(y), angle(angle), scale(scale) {}
    explicit Anchor(float x, float y, float angle, float scale, int segment)
        : x(x), y(y), angle(angle), scale(scale), segment(segment) {}
};

typedef std::vector<Anchor> Anchors;

}

#endif