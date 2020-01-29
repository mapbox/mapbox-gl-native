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
    GeometryCoordinates(std::initializer_list<GeometryCoordinate> args) : std::vector<GeometryCoordinate>(args) {}
};

class GeometryCollection : public std::vector<GeometryCoordinates> {
public:
    using coordinate_type = int16_t;
    template <class... Args>
    GeometryCollection(Args&&... args) : std::vector<GeometryCoordinates>(std::forward<Args>(args)...) {}
    GeometryCollection(std::initializer_list<GeometryCoordinates> args) : std::vector<GeometryCoordinates>(args) {}
    GeometryCollection(GeometryCollection&&) = default;
    GeometryCollection& operator=(GeometryCollection&&) = default;

    GeometryCollection clone() const { return GeometryCollection(*this); }

private:
    GeometryCollection(const GeometryCollection&) = default;
};

class GeometryTileFeature {
public:
    virtual ~GeometryTileFeature() = default;
    virtual FeatureType getType() const = 0;
    virtual optional<Value> getValue(const std::string& key) const = 0;
    virtual const PropertyMap& getProperties() const;
    virtual FeatureIdentifier getID() const { return NullValue {}; }
    virtual const GeometryCollection& getGeometries() const;
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

Feature::geometry_type convertGeometry(const GeometryTileFeature& geometryTileFeature, const CanonicalTileID& tileID);

// convert from GeometryTileFeature to Feature (eventually we should eliminate GeometryTileFeature)
Feature convertFeature(const GeometryTileFeature&, const CanonicalTileID&);

// Fix up possibly-non-V2-compliant polygon geometry using angus clipper.
// The result is guaranteed to have correctly wound, strictly simple rings.
GeometryCollection fixupPolygons(const GeometryCollection&);

struct ToGeometryCollection {
    GeometryCollection operator()(const mapbox::geometry::empty&) const {
        return GeometryCollection();
    }
    GeometryCollection operator()(const mapbox::geometry::point<int16_t>& geom) const {
        return { { geom } };
    }
    GeometryCollection operator()(const mapbox::geometry::multi_point<int16_t>& geom) const {
        return { geom };
    }
    GeometryCollection operator()(const mapbox::geometry::line_string<int16_t>& geom) const {
        return { geom };
    }
    GeometryCollection operator()(const mapbox::geometry::multi_line_string<int16_t>& geom) const {
        GeometryCollection collection;
        collection.reserve(geom.size());
        for (const auto& ring : geom) {
            collection.emplace_back(ring);
        }
        return collection;
    }
    GeometryCollection operator()(const mapbox::geometry::polygon<int16_t>& geom) const {
        GeometryCollection collection;
        collection.reserve(geom.size());
        for (const auto& ring : geom) {
            collection.emplace_back(ring);
        }
        return collection;
    }
    GeometryCollection operator()(const mapbox::geometry::multi_polygon<int16_t>& geom) const {
        GeometryCollection collection;
        for (const auto& polygon : geom) {
            for (const auto& ring : polygon) {
                collection.emplace_back(ring);
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
