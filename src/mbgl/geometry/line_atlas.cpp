#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/platform.hpp>

#include <sstream>
#include <cmath>

using namespace mbgl;

LineAtlas::LineAtlas(uint16_t w, uint16_t h)
    : width(w),
      height(h),
      data(new char[w * h]),
      dirty(true) {
}

LineAtlas::~LineAtlas() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    MBGL_CHECK_ERROR(glDeleteTextures(1, &texture));
    texture = 0;
}

LinePatternPos LineAtlas::getDashPosition(const std::vector<float> &dasharray, bool round) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    std::ostringstream sskey;

    for (const float &part : dasharray) {
        sskey << part << "-";
    }
    sskey << round;
    std::string key = sskey.str();

    if (positions.find(key) == positions.end()) {
        positions[key] = addDash(dasharray, round);
    }

    return positions[key];
}

LinePatternPos LineAtlas::addDash(const std::vector<float> &dasharray, bool round) {

    int n = round ? 7 : 0;
    int dashheight = 2 * n + 1;
    const uint8_t offset = 128;

    if (nextRow + dashheight > height) {
        fprintf(stderr, "[WARNING] line atlas bitmap overflow\n");
        return LinePatternPos();
    }

    float length = 0;
    for (const float &part : dasharray) {
        length += part;
    }

    float stretch = width / length;
    float halfWidth = stretch * 0.5;

    for (int y = -n; y <= n; y++) {
        int row = nextRow + n + y;
        int index = width * row;

        float left = 0;
        float right = dasharray[0];
        int partIndex = 1;

        for (int x = 0; x < width; x++) {

            while (right < x / stretch) {
                left = right;
                right = right + dasharray[partIndex];
                partIndex++;
            }

            float distLeft = fabs(x - left * stretch);
            float distRight = fabs(x - right * stretch);
            float dist = fmin(distLeft, distRight);
            bool inside = (partIndex % 2) == 1;
            int signedDistance;

            if (round) {
                float distMiddle = n ? (float)y / n * (halfWidth + 1) : 0;
                if (inside) {
                    float distEdge = halfWidth - fabs(distMiddle);
                    signedDistance = sqrt(dist * dist + distEdge * distEdge);
                } else {
                    signedDistance = halfWidth - sqrt(dist * dist + distMiddle * distMiddle);
                }

            } else {
                signedDistance = int((inside ? 1 : -1) * dist);
            }

            data[index + x] = fmax(0, fmin(255, signedDistance + offset));
        }
    }

    LinePatternPos position;
    position.y = (0.5 + nextRow + n) / height;
    position.height = (2.0 * n) / height;
    position.width = length;

    nextRow += dashheight;

    dirty = true;
    bind();

    return position;
};

void LineAtlas::bind() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    bool first = false;
    if (!texture) {
        MBGL_CHECK_ERROR(glGenTextures(1, &texture));
        MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        first = true;
    } else {
        MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
    }

    if (dirty) {
        if (first) {
            glTexImage2D(
                GL_TEXTURE_2D, // GLenum target
                0, // GLint level
                GL_ALPHA, // GLint internalformat
                width, // GLsizei width
                height, // GLsizei height
                0, // GLint border
                GL_ALPHA, // GLenum format
                GL_UNSIGNED_BYTE, // GLenum type
                data // const GLvoid * data
            );
        } else {
            glTexSubImage2D(
                GL_TEXTURE_2D, // GLenum target
                0, // GLint level
                0, // GLint xoffset
                0, // GLint yoffset
                width, // GLsizei width
                height, // GLsizei height
                GL_ALPHA, // GLenum format
                GL_UNSIGNED_BYTE, // GLenum type
                data // const GLvoid *pixels
            );
        }


        dirty = false;
    }
};
