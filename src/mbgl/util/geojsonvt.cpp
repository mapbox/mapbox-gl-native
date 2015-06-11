#include "geojsonvt.hpp"
#include "geojsonvt_clip.hpp"
#include "geojsonvt_convert.hpp"
#include "geojsonvt_util.hpp"

#include <queue>

namespace mapbox { namespace util { namespace geojsonvt {

std::unordered_map<std::string, clock_t> Time::activities;

#pragma mark - GeoJSONVT

std::vector<ProjectedFeature> GeoJSONVT::convertFeatures(const std::string &data, uint8_t baseZoom, double tolerance, bool debug) {

    if (debug) {
        Time::time("preprocess data");
    }

    uint32_t z2 = 1 << baseZoom;

    JSDocument deserializedData;
    deserializedData.Parse<0>(data.c_str());

    if (deserializedData.HasParseError()) {
        printf("invalid GeoJSON\n");
        return std::vector<ProjectedFeature>();
    }

    const uint16_t extent = 4096;

    std::vector<ProjectedFeature> features = Convert::convert(deserializedData, tolerance / (z2 * extent));

    if (debug) {
        Time::timeEnd("preprocess data");
    }

    return features;
}

GeoJSONVT::GeoJSONVT(const std::vector<ProjectedFeature>& features_, uint8_t baseZoom_, uint8_t maxZoom_, uint32_t maxPoints_, double tolerance_, bool debug_)
    : baseZoom(baseZoom_),
      maxZoom(maxZoom_),
      maxPoints(maxPoints_),
      tolerance(tolerance_),
      debug(debug_) {

    if (this->debug) {
        Time::time("generate tiles up to z" + std::to_string(maxZoom));
    }

    splitTile(features_, 0, 0, 0);

    if (this->debug) {
        printf("features: %i, points: %i\n", this->tiles[0].numFeatures, this->tiles[0].numPoints);
        Time::timeEnd("generate tiles up to z" + std::to_string(maxZoom));
        printf("tiles generated: %i {\n", this->total);
        for (const auto &pair : this->stats) {
            printf("    z%i: %i\n", pair.first, pair.second);
        }
        printf("}\n");
    }
}

void GeoJSONVT::splitTile(std::vector<ProjectedFeature> features_, uint8_t z_, uint32_t x_, uint32_t y_, int8_t cz, int32_t cx, int32_t cy) {

    std::queue<FeatureStackItem> stack;
    stack.emplace(features_, z_, x_, y_);

    while (stack.size()) {
        FeatureStackItem set = stack.front();
        stack.pop();
        std::vector<ProjectedFeature> features = std::move(set.features);
        uint8_t z = set.z;
        uint32_t x = set.x;
        uint32_t y = set.y;

        uint32_t z2 = 1 << z;
        const uint64_t id = toID(z, x, y);
        Tile* tile;
        double tileTolerance = (z == this->baseZoom ? 0 : this->tolerance / (z2 * this->extent));

        if (this->tiles.count(id)) {
            tile = &this->tiles[id];
        } else {
            if (this->debug) {
                Time::time("creation");
            }

            this->tiles[id] = std::move(Tile::createTile(features, z2, x, y, tileTolerance, extent, (z == this->baseZoom)));
            tile = &this->tiles[id];

            if (this->debug) {
                printf("tile z%i-%i-%i (features: %i, points: %i, simplified: %i\n", z, x, y,
                       tile->numFeatures, tile->numPoints, tile->numSimplified);
                Time::timeEnd("creation");

                uint8_t key = z;
                this->stats[key] = (this->stats.count(key) ? this->stats[key] + 1 : 1);
                this->total++;
            }
        }

        if ((cz < 0 && (z == this->maxZoom || this->tiles[id].numPoints <= this->maxPoints ||
            isClippedSquare(tile->features, this->extent, this->buffer))) || z == this->baseZoom || z == cz) {
            tile->source = std::vector<ProjectedFeature>(features);
            continue;
        }

        if (cz >= 0) {
            tile->source = std::vector<ProjectedFeature>(features);
        } else {
            tile->source = {};
        }

        if (this->debug) {
            Time::time("clipping");
        }

        double k1 = 0.5 * this->buffer / this->extent;
        double k2 = 0.5 - k1;
        double k3 = 0.5 + k1;
        double k4 = 1 + k1;

        std::vector<ProjectedFeature> tl;
        std::vector<ProjectedFeature> bl;
        std::vector<ProjectedFeature> tr;
        std::vector<ProjectedFeature> br;
        std::vector<ProjectedFeature> left;
        std::vector<ProjectedFeature> right;
        uint32_t m = 0;
        bool goLeft = false;
        bool goTop = false;

        if (cz >= 0) {
            m = 1 << (cz - z);
            goLeft = double(cx) / double(m) - double(x) < 0.5;
            goTop  = double(cy) / double(m) - double(y) < 0.5;
        }

        if (cz < 0 || goLeft) {
            left = Clip::clip(features, z2, x - k1, x + k3, 0, intersectX);
        }

        if (cz < 0 || !goLeft) {
            right = Clip::clip(features, z2, x + k2, x + k4, 0, intersectX);
        }

        if (left.size()) {
            if (cz < 0 || goTop) {
                tl = Clip::clip(left, z2, y - k1, y + k3, 1, intersectY);
            }

            if (cz < 0 || !goTop) {
                bl = Clip::clip(left, z2, y + k2, y + k4, 1, intersectY);
            }
        }

        if (right.size()) {
            if (cz < 0 || goTop) {
                tr = Clip::clip(right, z2, y - k1, y + k3, 1, intersectY);
            }

            if (cz < 0 || !goTop) {
                br = Clip::clip(right, z2, y + k2, y + k4, 1, intersectY);
            }
        }

        if (this->debug) {
            Time::timeEnd("clipping");
        }

        if (tl.size()) stack.emplace(std::move(tl), z + 1, x * 2,     y * 2);
        if (bl.size()) stack.emplace(std::move(bl), z + 1, x * 2,     y * 2 + 1);
        if (tr.size()) stack.emplace(std::move(tr), z + 1, x * 2 + 1, y * 2);
        if (br.size()) stack.emplace(std::move(br), z + 1, x * 2 + 1, y * 2 + 1);
    }
}

Tile& GeoJSONVT::getTile(uint8_t z, uint32_t x, uint32_t y) {

    std::lock_guard<std::mutex> lock(mtx);

    const uint64_t id = toID(z, x, y);
    if (this->tiles.count(id)) {
        return this->tiles[id];
    }

    if (this->debug) {
        printf("drilling down to z%i-%i-%i\n", z, x, y);
    }

    uint8_t z0 = z;
    uint32_t x0 = x;
    uint32_t y0 = y;
    Tile *parent = nullptr;

    while (!parent && z0) {
        z0--;
        x0 = x0 / 2;
        y0 = y0 / 2;
        const uint64_t checkID = toID(z0, x0, y0);
        if (this->tiles.count(checkID)) {
            parent = &this->tiles[checkID];
        }
    }

    if (this->debug) {
        printf("found parent tile z%i-%i-%i\n", z0, x0, y0);
    }

    if (parent->source.size()) {
        if (isClippedSquare(parent->features, this->extent, this->buffer)) {
            return *parent;
        }

        if (this->debug) {
            Time::time("drilling down");
        }

        splitTile(parent->source, z0, x0, y0, z, x, y);

        if (this->debug) {
            Time::timeEnd("drilling down");
        }
    }

    return this->tiles[id];
}

bool GeoJSONVT::isClippedSquare(const std::vector<TileFeature> &features, uint16_t extent_, uint8_t buffer_) const {

    if (features.size() != 1) {
        return false;
    }

    const TileFeature feature = features.front();

    if (feature.type != TileFeatureType::Polygon || feature.geometry.size() > 1) {
        return false;
    }

    const TileRing *ring = &(feature.geometry.front().get<TileRing>());

    for (size_t i = 0; i < ring->points.size(); ++i) {
        const TilePoint *p = &ring->points[i];
        if ((p->x != -buffer_ && p->x != extent_ + buffer_) ||
            (p->y != -buffer_ && p->y != extent_ + buffer_)) {
            return false;
        }
    }

    return true;
}

uint64_t GeoJSONVT::toID(uint8_t z, uint32_t x, uint32_t y) {

    return (((1 << z) * y + x) * 32) + z;
}

ProjectedPoint GeoJSONVT::intersectX(const ProjectedPoint &a, const ProjectedPoint &b, double x) {

    double r1 = x;
    double r2 = (x - a.x) * (b.y - a.y) / (b.x - a.x) + a.y;
    double r3 = 1;

    return ProjectedPoint(r1, r2, r3);
}

ProjectedPoint GeoJSONVT::intersectY(const ProjectedPoint &a, const ProjectedPoint &b, double y) {

    double r1 = (y - a.y) * (b.x - a.x) / (b.y - a.y) + a.x;
    double r2 = y;
    double r3 = 1;

    return ProjectedPoint(r1, r2, r3);
}

} /* namespace geojsonvt */ } /* namespace util */ } /* namespace mapbox */
