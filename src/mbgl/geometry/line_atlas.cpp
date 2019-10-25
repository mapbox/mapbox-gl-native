#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/gfx/upload_pass.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/util/hash.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>

#include <cmath>

namespace mbgl {
namespace {

size_t getDashPatternHash(const std::vector<float>& dasharray, const LinePatternCap patternCap) {
    size_t key =
        patternCap == LinePatternCap::Round ? std::numeric_limits<size_t>::min() : std::numeric_limits<size_t>::max();
    for (const float part : dasharray) {
        util::hash_combine<float>(key, part);
    }
    return key;
}

LinePatternPos addDashPattern(AlphaImage& image,
                              const int32_t yOffset,
                              const std::vector<float>& dasharray,
                              const LinePatternCap patternCap) {
    const uint8_t n = patternCap == LinePatternCap::Round ? 7 : 0;
    constexpr const uint8_t offset = 128;

    if (dasharray.size() < 2) {
        Log::Warning(Event::ParseStyle, "line dasharray requires at least two elements");
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
        int row = yOffset + n + y;
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
    position.y = (0.5 + yOffset + n) / image.size.height;
    position.height = (2.0 * n + 1) / image.size.height;
    position.width = length;

    return position;
}

} // namespace

DashPatternTexture::DashPatternTexture(const std::vector<float>& from_,
                                       const std::vector<float>& to_,
                                       const LinePatternCap cap) {
    const bool patternsIdentical = from_ == to_;
    const int32_t patternHeight = cap == LinePatternCap::Round ? 15 : 1;

    AlphaImage image({256, static_cast<uint32_t>((patternsIdentical ? 1 : 2) * patternHeight)});

    from = addDashPattern(image, 0, from_, cap);
    to = patternsIdentical ? from : addDashPattern(image, patternHeight, to_, cap);

    texture = std::move(image);
}

void DashPatternTexture::upload(gfx::UploadPass& uploadPass) {
    if (texture.is<AlphaImage>()) {
        texture = uploadPass.createTexture(texture.get<AlphaImage>());
    }
}

gfx::TextureBinding DashPatternTexture::textureBinding() const {
    // The texture needs to have been uploaded already.
    assert(texture.is<gfx::Texture>());
    return {texture.get<gfx::Texture>().getResource(),
            gfx::TextureFilterType::Linear,
            gfx::TextureMipMapType::No,
            gfx::TextureWrapType::Repeat,
            gfx::TextureWrapType::Clamp};
}

Size DashPatternTexture::getSize() const {
    return texture.match([](const auto& obj) { return obj.size; });
}

LineAtlas::LineAtlas() = default;

LineAtlas::~LineAtlas() = default;

DashPatternTexture& LineAtlas::getDashPatternTexture(const std::vector<float>& from,
                                                     const std::vector<float>& to,
                                                     const LinePatternCap cap) {
    const size_t hash = util::hash(getDashPatternHash(from, cap), getDashPatternHash(to, cap));

    // Note: We're not handling hash collisions here.
    const auto it = textures.find(hash);
    if (it == textures.end()) {
        auto inserted = textures.emplace(
            std::piecewise_construct, std::forward_as_tuple(hash), std::forward_as_tuple(from, to, cap));
        assert(inserted.second);
        needsUpload.emplace_back(hash);
        return inserted.first->second;
    } else {
        return it->second;
    }
}

void LineAtlas::upload(gfx::UploadPass& uploadPass) {
    for (const size_t hash : needsUpload) {
        const auto it = textures.find(hash);
        if (it != textures.end()) {
            it->second.upload(uploadPass);
        }
    }
    needsUpload.clear();
}

} // namespace mbgl
