#ifndef llmr_util_vec2_
#define llmr_util_vec2_

namespace llmr {

template <typename T = double>
struct vec2 {
    T x, y;

    // inline bool operator==(const vec2& rhs) const {
    //     return x == rhs.x && y == rhs.y;
    // }
};

template <typename T = double>
struct vec3 {
    T x, y, z;

    // inline bool operator==(const vec3& rhs) const {
    //     return x == rhs.x && y == rhs.y && z == rhs.z;
    // }
};

template <typename T = double>
struct vec4 {
    T x, y, z, w;

    // inline bool operator==(const vec4& rhs) const {
    //     return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    // }
};


struct box {
    vec2<double> tl, tr, bl, br;
};

}

#endif
