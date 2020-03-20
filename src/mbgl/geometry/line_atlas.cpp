#include <cmath>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/gfx/upload_pass.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/util/hash.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>

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

std::vector<DashRange> getDashRanges(const std::vector<float>& dasharray, float stretch) {
    // If dasharray has an odd length, both the first and last parts
    // are dashes and should be joined seamlessly.
    const bool oddDashArray = dasharray.size() % 2 == 1;

    float left = oddDashArray ? -dasharray.back() * stretch : 0.0f;
    float right = dasharray.front() * stretch;
    bool isDash = true;

    std::vector<DashRange> ranges;
    ranges.reserve(dasharray.size());
    ranges.push_back({left, right, isDash, dasharray.front() == 0.0f});

    float currentDashLength = dasharray.front();
    for (size_t i = 1; i < dasharray.size(); ++i) {
        isDash = !isDash;

        const float dashLength = dasharray[i];
        left = currentDashLength * stretch;
        currentDashLength += dashLength;
        right = currentDashLength * stretch;

        ranges.push_back({left, right, isDash, dashLength == 0.0f});
    }

    return ranges;
}

void addRoundDash(
    const std::vector<DashRange>& ranges, uint32_t yOffset, float stretch, const int n, AlphaImage& image) {
    const float halfStretch = stretch * 0.5f;

    if (ranges.empty()) return;

    for (int y = -n; y <= n; y++) {
        int row = yOffset + n + y;
        const uint32_t index = image.size.width * row;
        uint32_t currIndex = 0;
        DashRange range = ranges[currIndex];

        for (uint32_t x = 0; x < image.size.width; ++x) {
            if (x / range.right > 1.0f && ++currIndex < ranges.size()) {
                range = ranges[currIndex];
            }

            float distLeft = fabsf(x - range.left);
            float distRight = fabsf(x - range.right);
            float minDist = fminf(distLeft, distRight);
            float signedDistance;

            float distMiddle = static_cast<float>(y) / n * (halfStretch + 1.0f);
            if (range.isDash) {
                float distEdge = halfStretch - fabsf(distMiddle);
                signedDistance = sqrtf(minDist * minDist + distEdge * distEdge);
            } else {
                signedDistance = halfStretch - sqrtf(minDist * minDist + distMiddle * distMiddle);
            }

            image.data[index + x] = static_cast<uint8_t>(fmaxf(0.0f, fminf(255.0f, signedDistance + 128.0f)));
        }
    }
}

void addRegularDash(std::vector<DashRange>& ranges, uint32_t yOffset, AlphaImage& image) {
    // Collapse any zero-length range
    for (auto it = ranges.begin(); it != ranges.end();) {
        if (it->isZeroLength) {
            it = ranges.erase(it);
        } else {
            ++it;
        }
    }

    if (ranges.empty()) return;

    if (ranges.size() > 1) {
        // Collapse neighbouring same-type parts into a single part
        for (auto curr = ranges.begin(), next = ranges.begin() + 1; next != ranges.end();) {
            if (next->isDash == curr->isDash) {
                next->left = curr->left;
                curr = ranges.erase(curr);
            } else {
                ++curr;
            }
            next = curr + 1;
        }
    }

    DashRange& first = ranges.front();
    DashRange& last = ranges.back();
    if (first.isDash == last.isDash) {
        first.left = last.left - image.size.width;
        last.right = first.right + image.size.width;
    }

    const uint32_t index = image.size.width * yOffset;
    uint32_t currIndex = 0;
    DashRange range = ranges[currIndex];

    for (uint32_t x = 0; x < image.size.width; ++x) {
        if (x / range.right > 1.0f && ++currIndex < ranges.size()) {
            range = ranges[currIndex];
        }

        float distLeft = fabsf(x - range.left);
        float distRight = fabsf(x - range.right);
        float minDist = fminf(distLeft, distRight);
        float signedDistance = range.isDash ? minDist : -minDist;

        image.data[index + x] = static_cast<uint8_t>(fmaxf(0.0f, fminf(255.0f, signedDistance + 128.0f)));
    }
}

LinePatternPos addDashPattern(AlphaImage& image,
                              uint32_t yOffset,
                              const std::vector<float>& dasharray,
                              const LinePatternCap patternCap) {
    const uint8_t n = patternCap == LinePatternCap::Round ? 7 : 0;

    if (dasharray.size() < 2) {
        Log::Warning(Event::ParseStyle, "line dasharray requires at least two elements");
        return {};
    }

    float length = 0;
    for (const float part : dasharray) {
        length += part;
    }

    float stretch = image.size.width / length;
    std::vector<DashRange> ranges = getDashRanges(dasharray, stretch);

    if (patternCap == LinePatternCap::Round) {
        addRoundDash(ranges, yOffset, stretch, n, image);
    } else {
        addRegularDash(ranges, yOffset, image);
    }

    LinePatternPos position;
    position.y = (0.5f + yOffset + n) / image.size.height;
    position.height = (2.0f * n + 1) / image.size.height;
    position.width = length;

    return position;
}

} // namespace

DashPatternTexture::DashPatternTexture(const std::vector<float>& from_,
                                       const std::vector<float>& to_,
                                       const LinePatternCap cap) {
    const bool patternsIdentical = from_ == to_;
    const uint32_t patternHeight = cap == LinePatternCap::Round ? 15 : 1;
    const uint32_t height = (patternsIdentical ? 1 : 2) * patternHeight;

    // The OpenGL ES 2.0 spec, section 3.8.2 states:
    //
    //     Calling a sampler from a fragment shader will return (R,G,B,A) = (0,0,0,1) if any of the
    //     following conditions are true:
    //     […]
    //     - A two-dimensional sampler is called, the corresponding texture image is a
    //       non-power-of-two image […], and either the texture wrap mode is not CLAMP_TO_EDGE, or
    //       the minification filter is neither NEAREST nor LINEAR.
    //     […]
    //
    // This means that texture lookups won't work for NPOT textures unless they use GL_CLAMP_TO_EDGE.
    // We're using GL_CLAMP_TO_EDGE for the vertical direction, but GL_REPEAT for the horizontal
    // direction, which means that we need a power-of-two texture for our line dash patterns to work
    // on OpenGL ES 2.0 conforming implementations. Fortunately, this just means changing the height
    // from 15 to 16, and from 30 to 32, so we don't waste many pixels.
    const uint32_t textureHeight = 1 << util::ceil_log2(height);
    AlphaImage image({256, textureHeight});

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
