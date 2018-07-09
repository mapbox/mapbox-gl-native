#pragma once

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/optional.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace mbgl {

class CanonicalTileID;

// Normalized vector tile coordinates.
// Each geometry coordinate represents a point in a bidimensional space,
// varying from -V...0...+V, where V is the maximum extent applicable.
using GeometryCoordinate = Point<int16_t>;

class GeometryCoordinates : public std::vector<GeometryCoordinate> {
public:
    using coordinate_type = int16_t;

    template <class... Args>
    GeometryCoordinates(Args&&... args) : std::vector<GeometryCoordinate>(std::forward<Args>(args)...) {}
    GeometryCoordinates(std::initializer_list<GeometryCoordinate> args)
      : std::vector<GeometryCoordinate>(std::move(args)) {}
};

class GeometryCollection : public std::vector<GeometryCoordinates> {
public:
    using coordinate_type = int16_t;
    template <class... Args>
    GeometryCollection(Args&&... args) : std::vector<GeometryCoordinates>(std::forward<Args>(args)...) {}
    GeometryCollection(std::initializer_list<GeometryCoordinates> args)
      : std::vector<GeometryCoordinates>(std::move(args)) {}
};

class GeometryTileFeature {
public:
    virtual ~GeometryTileFeature() = default;
    virtual FeatureType getType() const = 0;
    virtual optional<Value> getValue(const std::string& key) const = 0;
    virtual PropertyMap getProperties() const { return PropertyMap(); }
    virtual optional<FeatureIdentifier> getID() const { return {}; }
    virtual GeometryCollection getGeometries() const = 0;
};

class GeometryTileLayer {
public:
    virtual ~GeometryTileLayer() = default;
    virtual std::size_t featureCount() const = 0;

    // Returns the feature object at the given position within the layer. The returned feature
    // object may *not* outlive the layer object.
    virtual std::unique_ptr<GeometryTileFeature> getFeature(std::size_t) const = 0;

    virtual std::string getName() const = 0;
};

class GeometryTileData {
public:
    virtual ~GeometryTileData() = default;
    virtual std::unique_ptr<GeometryTileData> clone() const = 0;

    // Returns the layer with the given name. The returned layer object *may* outlive the data
    // object.
    virtual std::unique_ptr<GeometryTileLayer> getLayer(const std::string&) const = 0;
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

struct ToGeometryCollection {
    GeometryCollection operator()(const mapbox::geometry::point<int16_t>& geom) const {
        return { { geom } };
    }
    GeometryCollection operator()(const mapbox::geometry::multi_point<int16_t>& geom) const {
        GeometryCoordinates coordinates;
        coordinates.reserve(geom.size());
        for (const auto& point : geom) {
            coordinates.emplace_back(point);
        }
        return { coordinates };
    }
    GeometryCollection operator()(const mapbox::geometry::line_string<int16_t>& geom) const {
        GeometryCoordinates coordinates;
        coordinates.reserve(geom.size());
        for (const auto& point : geom) {
            coordinates.emplace_back(point);
        }
        return { coordinates };
    }
    GeometryCollection operator()(const mapbox::geometry::multi_line_string<int16_t>& geom) const {
        GeometryCollection collection;
        collection.reserve(geom.size());
        for (const auto& ring : geom) {
            GeometryCoordinates coordinates;
            coordinates.reserve(ring.size());
            for (const auto& point : ring) {
                coordinates.emplace_back(point);
            }
            collection.push_back(std::move(coordinates));
        }
        return collection;
    }
    GeometryCollection operator()(const mapbox::geometry::polygon<int16_t>& geom) const {
        GeometryCollection collection;
        collection.reserve(geom.size());
        for (const auto& ring : geom) {
            GeometryCoordinates coordinates;
            coordinates.reserve(ring.size());
            for (const auto& point : ring) {
                coordinates.emplace_back(point);
            }
            collection.push_back(std::move(coordinates));
        }
        return collection;
    }
    GeometryCollection operator()(const mapbox::geometry::multi_polygon<int16_t>& geom) const {
        GeometryCollection collection;
        for (auto& polygon : geom) {
            for (auto& ring : polygon) {
                GeometryCoordinates coordinates;
                coordinates.reserve(ring.size());
                for (auto& point : ring) {
                    coordinates.emplace_back(point);
                }
                collection.push_back(std::move(coordinates));
            }
        }
        return collection;
    }
    GeometryCollection operator()(const mapbox::geometry::geometry_collection<int16_t>&) const {
        GeometryCollection collection;
        return collection;
    }
};

} // namespace mbgl
