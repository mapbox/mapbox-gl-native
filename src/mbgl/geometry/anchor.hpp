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

    explicit Anchor(float x_, float y_, float angle_, float scale_)
        : x(x_), y(y_), angle(angle_), scale(scale_) {}
    explicit Anchor(float x_, float y_, float angle_, float scale_, int segment_)
        : x(x_), y(y_), angle(angle_), scale(scale_), segment(segment_) {}

    template <typename M>
    inline Anchor matMul(const M &m) const {
        return Anchor{m[0] * x + m[1] * y, m[2] * x + m[3] * y, angle, scale, segment};
    }
};

typedef std::vector<Anchor> Anchors;

}

#endif
