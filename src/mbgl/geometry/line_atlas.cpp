#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>

#include <boost/functional/hash.hpp>

#include <sstream>
#include <cmath>

namespace mbgl {

LineAtlas::LineAtlas(const Size size)
    : image(size),
      dirty(true) {
}

LineAtlas::~LineAtlas() = default;

LinePatternPos LineAtlas::getDashPosition(const std::vector<float>& dasharray,
                                          LinePatternCap patternCap) {
    size_t key = patternCap == LinePatternCap::Round ? std::numeric_limits<size_t>::min()
                                                     : std::numeric_limits<size_t>::max();
    for (const float part : dasharray) {
        boost::hash_combine<float>(key, part);
    }

    // Note: We're not handling hash collisions here.
    const auto it = positions.find(key);
    if (it == positions.end()) {
        auto inserted = positions.emplace(key, addDash(dasharray, patternCap));
        assert(inserted.second);
        return inserted.first->second;
    } else {
        return it->second;
    }
}

LinePatternPos LineAtlas::addDash(const std::vector<float>& dasharray, LinePatternCap patternCap) {
    const uint8_t n = patternCap == LinePatternCap::Round ? 7 : 0;
    const uint8_t dashheight = 2 * n + 1;
    const uint8_t offset = 128;

    if (dasharray.size() < 2) {
        return LinePatternPos();
    }

    if (nextRow + dashheight > image.size.height) {
        Log::Warning(Event::OpenGL, "line atlas bitmap overflow");
        return LinePatternPos();
    }

    float length = 0;
    for (const float part : dasharray) {
        length += part;
    }

    float stretch = image.size.width / length;
    float halfWidth = stretch * 0.5;
    // If dasharray has an odd length, both the first and last parts
    // are dashes and should be joined seamlessly.
    bool oddLength = dasharray.size() % 2 == 1;

    for (int y = -n; y <= n; y++) {
        int row = nextRow + n + y;
        int index = image.size.width * row;

        float left = 0;
        float right = dasharray[0];
        unsigned int partIndex = 1;

        if (oddLength) {
            left -= dasharray.back();
        }

        for (uint32_t x = 0; x < image.size.width; x++) {

            while (right < x / stretch) {
                left = right;
                if (partIndex >= dasharray.size()) {
                    return LinePatternPos();
                }
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

            if (patternCap == LinePatternCap::Round) {
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

            image.data[index + x] = fmax(0, fmin(255, signedDistance + offset));
        }
    }

    LinePatternPos position;
    position.y = (0.5 + nextRow + n) / image.size.height;
    position.height = (2.0 * n) / image.size.height;
    position.width = length;

    nextRow += dashheight;

    dirty = true;

    return position;
}

Size LineAtlas::getSize() const {
    return image.size;
}

void LineAtlas::upload(gl::Context& context, gl::TextureUnit unit) {
    if (!texture) {
        texture = context.createTexture(image, unit);
    } else if (dirty) {
        context.updateTexture(*texture, image, unit);
    }

    dirty = false;
}

void LineAtlas::bind(gl::Context& context, gl::TextureUnit unit) {
    upload(context, unit);
    context.bindTexture(*texture, unit, gl::TextureFilter::Linear, gl::TextureMipMap::No,
                        gl::TextureWrap::Repeat, gl::TextureWrap::Clamp);
}

} // namespace mbgl
