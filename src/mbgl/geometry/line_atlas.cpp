#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/util/gl_object_store.hpp>
#include <mbgl/util/thread_context.hpp>

#include <boost/functional/hash.hpp>

#include <sstream>
#include <cmath>

using namespace mbgl;

LineAtlas::LineAtlas(uint16_t w, uint16_t h)
    : width(w),
      height(h),
      data(std::make_unique<uint8_t[]>(w * h)),
      dirty(true) {
}

LineAtlas::~LineAtlas() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    if (texture) {
        mbgl::util::ThreadContext::getGLObjectStore()->abandonTexture(texture);
        texture = 0;
    }
}

LinePatternPos LineAtlas::getDashPosition(const std::vector<float> &dasharray, bool round) {
    size_t key = round ? std::numeric_limits<size_t>::min() : std::numeric_limits<size_t>::max();
    for (const float part : dasharray) {
        boost::hash_combine<float>(key, part);
    }

    // Note: We're not handling hash collisions here.

    std::lock_guard<std::recursive_mutex> lock(mtx);
    const auto it = positions.find(key);
    if (it == positions.end()) {
        auto inserted = positions.emplace(key, addDash(dasharray, round));
        assert(inserted.second);
        return inserted.first->second;
    } else {
        return it->second;
    }
}

LinePatternPos LineAtlas::addDash(const std::vector<float> &dasharray, bool round) {

    int n = round ? 7 : 0;
    int dashheight = 2 * n + 1;
    const uint8_t offset = 128;

    if (nextRow + dashheight > height) {
        Log::Warning(Event::OpenGL, "line atlas bitmap overflow");
        return LinePatternPos();
    }

    float length = 0;
    for (const float part : dasharray) {
        length += part;
    }

    float stretch = width / length;
    float halfWidth = stretch * 0.5;
    // If dasharray has an odd length, both the first and last parts
    // are dashes and should be joined seamlessly.
    bool oddLength = dasharray.size() % 2 == 1;

    for (int y = -n; y <= n; y++) {
        int row = nextRow + n + y;
        int index = width * row;

        float left = 0;
        float right = dasharray[0];
        unsigned int partIndex = 1;

        if (oddLength) {
            left -= dasharray.back();
        }

        for (int x = 0; x < width; x++) {

            while (right < x / stretch) {
                left = right;
                right = right + dasharray[partIndex];

                if (oddLength && partIndex == dasharray.size() - 1) {
                    right += dasharray.front();
                }

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

void LineAtlas::upload() {
    if (dirty) {
        bind();
    }
}

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
            MBGL_CHECK_ERROR(glTexImage2D(
                GL_TEXTURE_2D, // GLenum target
                0, // GLint level
                GL_ALPHA, // GLint internalformat
                width, // GLsizei width
                height, // GLsizei height
                0, // GLint border
                GL_ALPHA, // GLenum format
                GL_UNSIGNED_BYTE, // GLenum type
                data.get() // const GLvoid * data
            ));
        } else {
            MBGL_CHECK_ERROR(glTexSubImage2D(
                GL_TEXTURE_2D, // GLenum target
                0, // GLint level
                0, // GLint xoffset
                0, // GLint yoffset
                width, // GLsizei width
                height, // GLsizei height
                GL_ALPHA, // GLenum format
                GL_UNSIGNED_BYTE, // GLenum type
                data.get() // const GLvoid *pixels
            ));
        }


        dirty = false;
    }
};
