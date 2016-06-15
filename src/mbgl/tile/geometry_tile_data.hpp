#pragma once

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/constants.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace mbgl {

class CanonicalTileID;

// Normalized vector tile coordinates.
// Each geometry coordinate represents a point in a bidimensional space,
// varying from -V...0...+V, where V is the maximum extent applicable.
using GeometryCoordinate = Point<int16_t>;

class GeometryCoordinates : public std::vector<GeometryCoordinate> {
public:
    using coordinate_type = int16_t;
    using std::vector<GeometryCoordinate>::vector;
};

class GeometryCollection : public std::vector<GeometryCoordinates> {
public:
    using coordinate_type = int16_t;
    using std::vector<GeometryCoordinates>::vector;
};

class GeometryTileFeature : private util::noncopyable {
public:
    virtual ~GeometryTileFeature() = default;
    virtual FeatureType getType() const = 0;
    virtual optional<Value> getValue(const std::string& key) const = 0;
    virtual Feature::property_map getProperties() const { return Feature::property_map(); }
    virtual optional<uint64_t> getID() const { return {}; }
    virtual GeometryCollection getGeometries() const = 0;
};

class GeometryTileLayer : private util::noncopyable {
public:
    virtual ~GeometryTileLayer() = default;
    virtual std::size_t featureCount() const = 0;
    virtual util::ptr<const GeometryTileFeature> getFeature(std::size_t) const = 0;
    virtual std::string getName() const = 0;
};

class GeometryTileData : private util::noncopyable {
public:
    virtual ~GeometryTileData() = default;
    virtual util::ptr<GeometryTileLayer> getLayer(const std::string&) const = 0;
};

// classifies an array of rings into polygons with outer rings and holes
std::vector<GeometryCollection> classifyRings(const GeometryCollection&);

// Truncate polygon to the largest `maxHoles` inner rings by area.
void limitHoles(GeometryCollection&, uint32_t maxHoles);

// convert from GeometryTileFeature to Feature (eventually we should eliminate GeometryTileFeature)
Feature convertFeature(const GeometryTileFeature&, const CanonicalTileID&);

// Fix up possibly-non-V2-compliant polygon geometry using angus clipper.
// The result is guaranteed to have correctly wound, strictly simple rings.
GeometryCollection fixupPolygons(const GeometryCollection&);

} // namespace mbgl
