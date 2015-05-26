#include <mbgl/map/annotation.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/map/live_tile.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/std.hpp>

#include <algorithm>
#include <memory>

namespace mbgl {

class Annotation : private util::noncopyable {
    friend class AnnotationManager;
public:
    Annotation(AnnotationType, const AnnotationSegments&, const StyleProperties&);

private:
    LatLng getPoint() const;
    LatLngBounds getBounds() const { return bounds; }

private:
    const AnnotationType type = AnnotationType::Point;
    const AnnotationSegments geometry;
    const StyleProperties styleProperties;
    std::unordered_map<TileID, std::weak_ptr<const LiveTileFeature>, TileID::Hash> tileFeatures;
    const LatLngBounds bounds;
};

Annotation::Annotation(AnnotationType type_,
                       const AnnotationSegments& geometry_,
                       const StyleProperties& styleProperties_)
    : type(type_),
      geometry(geometry_),
      styleProperties(styleProperties_),
      bounds([this] {
          LatLngBounds bounds_;
          if (type == AnnotationType::Point) {
              bounds_ = { getPoint(), getPoint() };
          } else {
              for (auto& segment : geometry) {
                  for (auto& point : segment) {
                      bounds_.extend(point);
                  }
              }
          }
          return bounds_;
      }()) {
}

LatLng Annotation::getPoint() const {
    // Return the first line's first point. Shortcut for point annotations.
    assert(!geometry.empty());
    assert(!geometry[0].empty());
    return geometry[0][0];
}

AnnotationManager::AnnotationManager() {}

AnnotationManager::~AnnotationManager() {
    // leave this here because the header file doesn't have a definition of
    // Annotation so we can't destruct the object with just the header file.
}

void AnnotationManager::setDefaultPointAnnotationSymbol(const std::string& symbol) {
    std::lock_guard<std::mutex> lock(mtx);
    defaultPointAnnotationSymbol = symbol;
}

uint32_t AnnotationManager::nextID() {
    return nextID_++;
}

vec2<double> AnnotationManager::projectPoint(const LatLng& point) {
    // Project a coordinate into unit space in a square map.
    const double sine = std::sin(point.latitude * M_PI / 180.0);
    const double x = point.longitude / 360.0 + 0.5;
    const double y = 0.5 - 0.25 * std::log((1.0 + sine) / (1.0 - sine)) / M_PI;
    return { x, y };
}

std::pair<std::unordered_set<TileID, TileID::Hash>, AnnotationIDs>
AnnotationManager::addAnnotations(const AnnotationType type,
                                  const std::vector<AnnotationSegments>& segments,
                                  const StyleProperties& styleProperties,
                                  const AnnotationsProperties& annotationsProperties,
                                  const MapData& data) {
    std::lock_guard<std::mutex> lock(mtx);

    // We pre-generate tiles to contain each annotation up to the map's max zoom.
    // We do this for fast rendering without projection conversions on the fly, as well as
    // to simplify bounding box queries of annotations later. Tiles get invalidated when
    // annotations are added or removed in order to refresh the map render without
    // touching the base map underneath.

    AnnotationIDs annotationIDs;
    annotationIDs.reserve((type == AnnotationType::Shape ?
                           segments.size() :        // shapes
                           segments[0][0].size())); // points

    std::unordered_set<TileID, TileID::Hash> affectedTiles;

    const uint8_t maxZoom = data.transform.getMaxZoom();

    for (size_t s = 0; s < segments.size(); ++s) {
        auto& shape = segments[s];

        std::vector<std::vector<vec2<double>>> projectedShape;
        projectedShape.reserve(shape.size());

        const uint32_t annotationID = nextID();

        for (size_t l = 0; l < shape.size(); ++l) {
            auto& line = shape[l];

            std::vector<vec2<double>> projectedLine;
            projectedLine.reserve(line.size());

            for (size_t p = 0; p < line.size(); ++p) {
                auto& point = line[p];

                // projection conversion into unit space
                const auto pp = projectPoint(point);

                projectedLine.push_back(pp);
            }

            projectedShape.push_back(projectedLine);
        }

        std::unordered_map<std::string, std::string> featureProperties;

        if (type == AnnotationType::Point) {
            // at render time we style the point according to its {sprite} field
            const std::string& symbol = annotationsProperties.at("symbols")[s];
            if (symbol.length()) {
                featureProperties.emplace("sprite", symbol);
            } else {
                featureProperties.emplace("sprite", defaultPointAnnotationSymbol);
            }
        }

        auto featureAffectedTiles = addTileFeature(
            annotationID,
            shape,
            projectedShape,
            type,
            styleProperties,
            featureProperties,
            maxZoom
        );

        std::copy(featureAffectedTiles.begin(), featureAffectedTiles.end(), std::inserter(affectedTiles, affectedTiles.begin()));

        annotationIDs.push_back(annotationID);
    }

    // Tile:IDs that need refreshed and the annotation identifiers held onto by the client.
    return std::make_pair(affectedTiles, annotationIDs);
}

std::unordered_set<TileID, TileID::Hash>
AnnotationManager::addTileFeature(const uint32_t annotationID,
                                  const AnnotationSegments& segments,
                                  const std::vector<std::vector<vec2<double>>>& projectedFeature,
                                  const AnnotationType& type,
                                  const StyleProperties& styleProperties,
                                  const std::unordered_map<std::string, std::string>& featureProperties,
                                  const uint8_t maxZoom) {

    // track the annotation global ID and its original geometry
    auto anno_it = annotations.emplace(annotationID,
        util::make_unique<Annotation>(type, segments, styleProperties));

    // side length of map at max zoom
    uint32_t z2 = 1 << maxZoom;

    const uint16_t extent = 4096;

    uint32_t x = 0;
    uint32_t y = 0;

    std::unordered_set<TileID, TileID::Hash> affectedTiles;

    for (int8_t z = maxZoom; z >= 0; z--) {

        GeometryCollection geometries;

        if (type == AnnotationType::Point) {
            auto& pp = projectedFeature[0][0];

            x = pp.x * z2;
            y = pp.y * z2;

            const Coordinate coordinate(extent * (pp.x * z2 - x), extent * (pp.y * z2 - y));

            geometries = {{ {{ coordinate }} }};
        } else {
            for (size_t l = 0; l < projectedFeature.size(); ++l) {

                std::vector<Coordinate> line;

                for (size_t p = 0; p < projectedFeature[l].size(); ++p) {

                    auto& pp = projectedFeature[l][p];

                    x = pp.x * z2;
                    y = pp.y * z2;

                    const Coordinate coordinate(extent * (pp.x * z2 - x), extent * (pp.y * z2 - y));

                    line.push_back(coordinate);
                }

                geometries.push_back(line);
            }
        }

        auto tileID = TileID(z, x, y);

        // create tile feature
        auto feature = std::make_shared<const LiveTileFeature>(
            (type == AnnotationType::Point ? FeatureType::Point : FeatureType::LineString),
            geometries,
            featureProperties
        );

        // check for tile & create if necessary
        auto tile_pos = tiles.emplace(tileID,
                                      std::make_pair(std::unordered_set<uint32_t>({ annotationID }),
                                                     util::make_unique<LiveTile>()));

        // check for annotation layer & create if necessary
        util::ptr<LiveTileLayer> layer;
        auto& layerID = (type == AnnotationType::Point ? PointLayerID : ShapeLayerID);
        if (tile_pos.second || tile_pos.first->second.second->getMutableLayer(layerID) == nullptr) {
            layer = std::make_shared<LiveTileLayer>();
            tile_pos.first->second.second->addLayer(layerID, layer);
        } else {
            layer = tile_pos.first->second.second->getMutableLayer(layerID);

            // associate annotation with tile
            tile_pos.first->second.first.insert(annotationID);
        }

        // add feature to layer
        layer->addFeature(feature);

        // Record annotation association with tile and tile feature. This is used to determine stale tiles,
        // as well as to remove the feature from the tile upon annotation deletion.
        anno_it.first->second->tileFeatures.emplace(tileID, std::weak_ptr<const LiveTileFeature>(feature));

        // track affected tile
        affectedTiles.insert(tileID);

        // get ready for the next-lower zoom number
        z2 /= 2;
        x /= 2;
        y /= 2;
    }

    return affectedTiles;
}

std::pair<std::unordered_set<TileID, TileID::Hash>, AnnotationIDs>
AnnotationManager::addPointAnnotations(const AnnotationSegment& points,
                                       const AnnotationsProperties& annotationsProperties,
                                       const MapData& data) {
    return addAnnotations(AnnotationType::Point,
                          { { points } },
                          defaultStyleProperties<SymbolProperties>(),
                          annotationsProperties,
                          data);
}

std::pair<std::unordered_set<TileID, TileID::Hash>, AnnotationIDs>
AnnotationManager::addShapeAnnotations(const std::vector<AnnotationSegments>& shapes,
                                       const StyleProperties& styleProperties,
                                       const AnnotationsProperties& annotationsProperties,
                                       const MapData& data) {
    return addAnnotations(AnnotationType::Shape,
                          shapes,
                          styleProperties,
                          annotationsProperties,
                          data);
}

std::unordered_set<TileID, TileID::Hash> AnnotationManager::removeAnnotations(const AnnotationIDs& ids,
                                                         const MapData& data) {
    std::lock_guard<std::mutex> lock(mtx);

    std::unordered_set<TileID, TileID::Hash> affectedTiles;

    std::vector<uint32_t> z2s;
    const uint8_t zoomCount = data.transform.getMaxZoom() + 1;
    z2s.reserve(zoomCount);
    for (uint8_t z = 0; z < zoomCount; ++z) {
        z2s.emplace_back(1<< z);
    }

    LatLng latLng;
    vec2<double> p;
    uint32_t x, y;

    // iterate annotation id's passed
    for (const auto& annotationID : ids) {
        // grab annotation object
        const auto& annotation_it = annotations.find(annotationID);
        if (annotation_it != annotations.end()) {
            const auto& annotation = annotation_it->second;
            // calculate annotation's affected tile for each zoom
            for (uint8_t z = 0; z < zoomCount; ++z) {
                latLng = annotation->getPoint();
                p = projectPoint(latLng);
                x = z2s[z] * p.x;
                y = z2s[z] * p.y;
                TileID tid(z, x, y);
                // erase annotation from tile's list
                auto& tileAnnotations = tiles[tid].first;
                tileAnnotations.erase(annotationID);
                // remove annotation's features from tile
                const auto& features_it = annotation->tileFeatures.find(tid);
                if (features_it != annotation->tileFeatures.end()) {
                    const auto& layer =
                        tiles[tid].second->getMutableLayer(PointLayerID); //
                    layer->removeFeature(features_it->second);
                    affectedTiles.insert(tid);
                }
            }
            annotations.erase(annotationID);
        }
    }

    // TileIDs for tiles that need refreshed.
    return affectedTiles;
}

AnnotationIDs AnnotationManager::getAnnotationsInBounds(const LatLngBounds& queryBounds,
                                                        const MapData& data) const {
    std::lock_guard<std::mutex> lock(mtx);

    const uint8_t z = data.transform.getMaxZoom();
    const uint32_t z2 = 1 << z;
    const vec2<double> swPoint = projectPoint(queryBounds.sw);
    const vec2<double> nePoint = projectPoint(queryBounds.ne);

    // tiles number y from top down
    const TileID nwTile(z, swPoint.x * z2, nePoint.y * z2);
    const TileID seTile(z, nePoint.x * z2, swPoint.y * z2);

    AnnotationIDs matchingAnnotations;

    for (auto& tile : tiles) {
        TileID id = tile.first;
        if (id.z == z) {
            if (id.x >= nwTile.x && id.x <= seTile.x && id.y >= nwTile.y && id.y <= seTile.y) {
                if (id.x > nwTile.x && id.x < seTile.x && id.y > nwTile.y && id.y < seTile.y) {
                    // Trivial accept; this tile is completely inside the query bounds, so
                    // we'll return all of its annotations.
                    std::copy(tile.second.first.begin(), tile.second.first.end(),
                              std::back_inserter(matchingAnnotations));
                } else {
                    // This tile is intersected by the query bounds. We need to check the
                    // tile's annotations' bounding boxes individually.
                    std::copy_if(tile.second.first.begin(), tile.second.first.end(),
                                 std::back_inserter(matchingAnnotations),
                                 [&](const uint32_t annotationID) -> bool {
                        const auto it = annotations.find(annotationID);
                        if (it != annotations.end()) {
                            const LatLngBounds annoBounds = it->second->getBounds();
                            return (annoBounds.sw.latitude >= queryBounds.sw.latitude &&
                                    annoBounds.ne.latitude <= queryBounds.ne.latitude &&
                                    annoBounds.sw.longitude >= queryBounds.sw.longitude &&
                                    annoBounds.ne.longitude <= queryBounds.ne.longitude);
                        } else {
                            return false;
                        }
                    });
                }
            }
        }
    }

    return matchingAnnotations;
}

LatLngBounds AnnotationManager::getBoundsForAnnotations(const AnnotationIDs& ids) const {
    std::lock_guard<std::mutex> lock(mtx);

    LatLngBounds bounds;
    for (auto id : ids) {
        const auto annotation_it = annotations.find(id);
        if (annotation_it != annotations.end()) {
            bounds.extend(annotation_it->second->getPoint());
        }
    }

    return bounds;
}

const LiveTile* AnnotationManager::getTile(const TileID& id) {
    std::lock_guard<std::mutex> lock(mtx);

    const auto tile_it = tiles.find(id);
    if (tile_it != tiles.end()) {
        return tile_it->second.second.get();
    }
    return nullptr;
}

const std::string AnnotationManager::PointLayerID = "com.mapbox.annotations.points";
const std::string AnnotationManager::ShapeLayerID = "com.mapbox.annotations.shape";

}
