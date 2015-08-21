#include "scaling.hpp"

namespace {

using namespace mbgl;

inline uint8_t bilinearInterpolate(uint8_t tl, uint8_t tr, uint8_t bl, uint8_t br, double dx, double dy) {
    const double t = dx * (tr - tl) + tl;
    const double b = dx * (br - bl) + bl;
    return t + dy * (b - t);
}

template <size_t i>
inline const uint8_t& b(const uint32_t& w) {
    return reinterpret_cast<const uint8_t*>(&w)[i];
}

template <size_t i>
inline uint8_t& b(uint32_t& w) {
    return reinterpret_cast<uint8_t*>(&w)[i];
}

vec2<double> getFactor(const Rect<uint32_t>& srcPos, const Rect<uint32_t>& dstPos) {
    return {
        double(srcPos.w) / dstPos.w,
        double(srcPos.h) / dstPos.h
    };
}

vec2<uint32_t> getBounds(const vec2<uint32_t>& srcSize, const Rect<uint32_t>& srcPos,
                         const vec2<uint32_t>& dstSize, const Rect<uint32_t>& dstPos,
                         const vec2<double>& factor) {
    if (srcPos.x > srcSize.x || srcPos.y > srcSize.y ||
        dstPos.x > dstSize.x || dstPos.y > dstSize.y) {
        // Source or destination position is out of range.
        return { 0, 0 };
    }

    // Make sure we don't read/write values out of range.
    return { std::min(uint32_t(double(srcSize.x - srcPos.x) / factor.x),
                      std::min(dstSize.x - dstPos.x, dstPos.w)),
             std::min(uint32_t(double(srcSize.y - srcPos.y) / factor.y),
                      std::min(dstSize.y - dstPos.y, dstPos.h)) };
}
}

namespace mbgl {
namespace util {

void bilinearScale(const uint32_t* srcData, const vec2<uint32_t>& srcSize,
                   const Rect<uint32_t>& srcPos, uint32_t* dstData, const vec2<uint32_t>& dstSize,
                   const Rect<uint32_t>& dstPos, bool wrap) {
    const auto factor = getFactor(srcPos, dstPos);
    const auto bounds = getBounds(srcSize, srcPos, dstSize, dstPos, factor);

    uint32_t x, y;
    size_t i = dstSize.x * dstPos.y + dstPos.x;
    for (y = 0; y < bounds.y; y++) {
        const double fractY = y * factor.y;
        const uint32_t Y0 = fractY;
        const uint32_t Y1 = wrap ? (Y0 + 1) % srcPos.h : (Y0 + 1);
        const uint32_t srcY0 = srcPos.y + Y0;
        const uint32_t srcY1 = std::min(srcPos.y + Y1, srcSize.y - 1);
        for (x = 0; x < bounds.x; x++) {
            const double fractX = x * factor.x;
            const uint32_t X0 = fractX;
            const uint32_t X1 = wrap ? (X0 + 1) % srcPos.w : (X0 + 1);
            const uint32_t srcX0 = srcPos.x + X0;
            const uint32_t srcX1 = std::min(srcPos.x + X1, srcSize.x - 1);

            const uint32_t tl = srcData[srcSize.x * srcY0 + srcX0];
            const uint32_t tr = srcData[srcSize.x * srcY0 + srcX1];
            const uint32_t bl = srcData[srcSize.x * srcY1 + srcX0];
            const uint32_t br = srcData[srcSize.x * srcY1 + srcX1];

            const double dx = fractX - X0;
            const double dy = fractY - Y0;
            uint32_t& dst = dstData[i + x];
            b<0>(dst) = bilinearInterpolate(b<0>(tl), b<0>(tr), b<0>(bl), b<0>(br), dx, dy);
            b<1>(dst) = bilinearInterpolate(b<1>(tl), b<1>(tr), b<1>(bl), b<1>(br), dx, dy);
            b<2>(dst) = bilinearInterpolate(b<2>(tl), b<2>(tr), b<2>(bl), b<2>(br), dx, dy);
            b<3>(dst) = bilinearInterpolate(b<3>(tl), b<3>(tr), b<3>(bl), b<3>(br), dx, dy);
        }
        i += dstSize.x;
    }
}

void nearestNeighborScale(const uint32_t* srcData, const vec2<uint32_t>& srcSize,
                          const Rect<uint32_t>& srcPos, uint32_t* dstData,
                          const vec2<uint32_t>& dstSize, const Rect<uint32_t>& dstPos) {
    const auto factor = getFactor(srcPos, dstPos);
    const auto bounds = getBounds(srcSize, srcPos, dstSize, dstPos, factor);

    double fractSrcY = srcPos.y;
    double fractSrcX;
    size_t i = dstSize.x * dstPos.y + dstPos.x;
    uint32_t srcY;
    uint32_t x, y;
    for (y = 0; y < bounds.y; y++) {
        fractSrcX = srcPos.x;
        srcY = srcSize.x * uint32_t(fractSrcY);
        for (x = 0; x < bounds.x; x++) {
            dstData[i + x] = srcData[srcY + uint32_t(fractSrcX)];
            fractSrcX += factor.x;
        }
        i += dstSize.x;
        fractSrcY += factor.y;
    }
}

}
}
