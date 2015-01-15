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
}

LinePatternPos LineAtlas::getDashPosition(std::vector<float> &dasharray, bool round) {
    std::ostringstream sskey;

    for (const float &part : dasharray) {
        sskey << part << "-";
    }
    sskey << round;
    std::string key = sskey.str();

    if (positions.find(key) == positions.end()) {
        fprintf(stderr, "add %s\n", key.c_str());
        positions[key] = addDash(dasharray, round);
    }

    return positions[key];
}

LinePatternPos LineAtlas::addDash(std::vector<float> &dasharray, bool round) {

    int n = round ? 7 : 0;
    int dashheight = 2 * n + 1;
    const uint8_t offset = 128;

    // TODO check if enough space

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
                float distMiddle = n ? y / n * halfWidth : 0;
                float distEdge = halfWidth - fabs(distMiddle);
                if (inside) {
                    signedDistance = sqrt(dist * dist + distEdge * distEdge);
                } else {
                    signedDistance = halfWidth = sqrt(dist * dist + distMiddle * distMiddle);
                }

            } else {
                signedDistance = int((inside ? 1 : -1) * dist);
            }

            data[index + x] = fmax(0, fmin(255, signedDistance + offset));
        }
    }

    LinePatternPos position;
    position.y = (0.5 + nextRow + n) / height;
    position.height = 2 * n / height;
    position.width = length;

    nextRow += dashheight;

    dirty = true;
    bind();

    return position;
};

void LineAtlas::bind() {
    if (!texture) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        dirty = true;
    } else {
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    dirty = true;

    if (dirty) {
        // TODO use texsubimage for updates
        // TODO lock?
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
        dirty = false;
    }
};
