#ifndef MBGL_UTIL_GL_HELPER
#define MBGL_UTIL_GL_HELPER

#include <mbgl/platform/gl.hpp>

#include <array>

namespace {

template <typename T, T (*Create)(), void (*Destroy)(const T&)>
class Preserve {
public:
    Preserve() : data(Create()) {}
    ~Preserve() { Destroy(data); }

private:
    const T data;
};

inline bool getBlend() {
    return glIsEnabled(GL_BLEND);
}

inline void setBlend(const bool& enabled) {
    enabled ? MBGL_CHECK_ERROR(glEnable(GL_BLEND)) : MBGL_CHECK_ERROR(glDisable(GL_BLEND));
}

inline std::array<float, 4> getClearColor() {
    std::array<float, 4> color;
    MBGL_CHECK_ERROR(glGetFloatv(GL_COLOR_CLEAR_VALUE, color.data()));
    return color;
}

inline void setClearColor(const std::array<float, 4>& color) {
    MBGL_CHECK_ERROR(glClearColor(color[0], color[1], color[2], color[3]));
}


inline std::array<GLenum, 2> getBlendFunc() {
    GLint func[2];
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &func[0]);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &func[1]);
    return {{ static_cast<GLenum>(func[0]), static_cast<GLenum>(func[1]) }};
}

inline void setBlendFunc(const std::array<GLenum, 2>& func) {
    MBGL_CHECK_ERROR(glBlendFunc(func[0], func[1]));
}

#ifndef GL_ES_VERSION_2_0
inline std::array<double, 2> getPixelZoom() {
    std::array<double, 2> zoom;
    glGetDoublev(GL_ZOOM_X, &zoom[0]);
    glGetDoublev(GL_ZOOM_Y, &zoom[1]);
    return zoom;
}

inline void setPixelZoom(const std::array<double, 2>& func) {
    MBGL_CHECK_ERROR(glPixelZoom(func[0], func[1]));
}


inline std::array<double, 4> getRasterPos() {
    std::array<double, 4> pos;
    MBGL_CHECK_ERROR(glGetDoublev(GL_CURRENT_RASTER_POSITION, pos.data()));
    return pos;
}

inline void setRasterPos(const std::array<double, 4>& pos) {
    MBGL_CHECK_ERROR(glRasterPos4d(pos[0], pos[1], pos[2], pos[3]));
}
#endif
} // end anonymous namespace

namespace mbgl {
namespace gl {

using PreserveBlend = Preserve<bool, getBlend, setBlend>;
using PreserveClearColor = Preserve<std::array<float, 4>, getClearColor, setClearColor>;
using PreserveBlendFunc = Preserve<std::array<GLenum, 2>, getBlendFunc, setBlendFunc>;

#ifndef GL_ES_VERSION_2_0
using PreservePixelZoom = Preserve<std::array<double, 2>, getPixelZoom, setPixelZoom>;
using PreserveRasterPos = Preserve<std::array<double, 4>, getRasterPos, setRasterPos>;
#endif

}
}

#endif
