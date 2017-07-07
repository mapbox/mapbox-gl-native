#include <mbgl/util/tiny_sdf.hpp>

#include <mbgl/util/math.hpp>

#include <algorithm>

namespace mbgl {
namespace util {

namespace tinysdf {

static const double INF = 1e20;

// 1D squared distance transform
void edt1d(std::vector<double>& f,
           std::vector<double>& d,
           std::vector<int16_t>& v,
           std::vector<double>& z,
           uint32_t n) {
    v[0] = 0;
    z[0] = -INF;
    z[1] = +INF;

    for (uint32_t q = 1, k = 0; q < n; q++) {
        double s = ((f[q] + q * q) - (f[v[k]] + v[k] * v[k])) / (2 * q - 2 * v[k]);
        while (s <= z[k]) {
            k--;
            s = ((f[q] + q * q) - (f[v[k]] + v[k] * v[k])) / (2 * q - 2 * v[k]);
        }
        k++;
        v[k] = q;
        z[k] = s;
        z[k + 1] = +INF;
    }

    for (uint32_t q = 0, k = 0; q < n; q++) {
        while (z[k + 1] < q) k++;
        d[q] = (q - v[k]) * (q - v[k]) + f[v[k]];
    }
}


// 2D Euclidean distance transform by Felzenszwalb & Huttenlocher https://cs.brown.edu/~pff/dt/
void edt(std::vector<double>& data,
         uint32_t width,
         uint32_t height,
         std::vector<double>& f,
         std::vector<double>& d,
         std::vector<int16_t>& v,
         std::vector<double>& z) {
    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0; y < height; y++) {
            f[y] = data[y * width + x];
        }
        edt1d(f, d, v, z, height);
        for (uint32_t y = 0; y < height; y++) {
            data[y * width + x] = d[y];
        }
    }
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            f[x] = data[y * width + x];
        }
        edt1d(f, d, v, z, width);
        for (uint32_t x = 0; x < width; x++) {
            data[y * width + x] = std::sqrt(d[x]);
        }
    }
}

} // namespace tinysdf

AlphaImage transformRasterToSDF(const AlphaImage& rasterInput, double radius, double cutoff) {
    uint32_t size = rasterInput.size.width * rasterInput.size.height;
    uint32_t maxDimension = std::max(rasterInput.size.width, rasterInput.size.height);
    
    AlphaImage sdf(rasterInput.size);
    
    // temporary arrays for the distance transform
    std::vector<double> gridOuter(size);
    std::vector<double> gridInner(size);
    std::vector<double> f(maxDimension);
    std::vector<double> d(maxDimension);
    std::vector<double> z(maxDimension + 1);
    std::vector<int16_t> v(maxDimension);
    
    for (uint32_t i = 0; i < size; i++) {
        double a = double(rasterInput.data[i]) / 255; // alpha value
        gridOuter[i] = a == 1.0 ? 0.0 : a == 0.0 ? tinysdf::INF : std::pow(std::max(0.0, 0.5 - a), 2.0);
        gridInner[i] = a == 1.0 ? tinysdf::INF : a == 0.0 ? 0.0 : std::pow(std::max(0.0, a - 0.5), 2.0);
    }

    tinysdf::edt(gridOuter, rasterInput.size.width, rasterInput.size.height, f, d, v, z);
    tinysdf::edt(gridInner, rasterInput.size.width, rasterInput.size.height, f, d, v, z);

    for (uint32_t i = 0; i < size; i++) {
        double distance = gridOuter[i] - gridInner[i];
        sdf.data[i] = std::max(0l, std::min(255l, std::lround(255.0 - 255.0 * (distance / radius + cutoff))));
    }

    return sdf;
}

} // namespace util
} // namespace mbgl
