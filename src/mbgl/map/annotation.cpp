#include <mbgl/map/annotation.hpp>
#include <mbgl/annotation/point_annotation.hpp>
#include <mbgl/annotation/shape_annotation.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/map/live_tile.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/geojsonvt/geojsonvt_convert.hpp>
#include <mbgl/util/ptr.hpp>

#include <algorithm>
#include <memory>

namespace mbgl {

Annotation::Annotation(AnnotationType type_,
                       const AnnotationSegments& geometry_,
                       const StyleProperties& styleProperties_)
    : styleProperties(styleProperties_),
      type(type_),
      geometry(geometry_),
      bounds([this] {
          LatLngBounds bounds_;
          assert(type != AnnotationType::Any);
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

void AnnotationManager::markStaleTiles(std::unordered_set<TileID, TileID::Hash> ids) {
    std::copy(ids.begin(), ids.end(), std::inserter(staleTiles, staleTiles.begin()));
}

std::unordered_set<TileID, TileID::Hash> AnnotationManager::resetStaleTiles() {
    return std::move(staleTiles);
}

void AnnotationManager::setDefaultPointAnnotationSymbol(const std::string& symbol) {
    defaultPointAnnotationSymbol = symbol;
}

uint32_t AnnotationManager::nextID() {
    return nextID_++;
}

vec2<double> AnnotationManager::projectPoint(const LatLng& point) {
    // Clamp to the latitude limits of Mercator.
    const double constrainedLatitude = std::fmin(std::fmax(point.latitude, -util::LATITUDE_MAX), util::LATITUDE_MAX);

    // Project a coordinate into unit space in a square map.
    const double sine = std::sin(constrainedLatitude * M_PI / 180.0);
    const double x = point.longitude / 360.0 + 0.5;
    const double y = 0.5 - 0.25 * std::log((1.0 + sine) / (1.0 - sine)) / M_PI;
    return { x, y };
}

std::unordered_set<TileID, TileID::Hash>
AnnotationManager::addTileFeature(const uint32_t annotationID,
                                  const AnnotationSegments& segments,
                                  const std::vector<std::vector<vec2<double>>>& projectedFeature,
                                  const AnnotationType& type,
                                  const StyleProperties& styleProperties,
                                  const std::unordered_map<std::string, std::string>& featureProperties,
                                  const uint8_t maxZoom) {

    assert(type != AnnotationType::Any);

    // track the annotation global ID and its original geometry
    auto anno_it = annotations.emplace(annotationID,
        std::make_unique<Annotation>(type, segments, styleProperties));

    std::unordered_set<TileID, TileID::Hash> affectedTiles;

    if (type == AnnotationType::Shape) {

        orderedShapeAnnotations.push_back(annotationID);

        using namespace mapbox::util::geojsonvt;

        const uint32_t z2 = 1 << maxZoom;
        const double baseTolerance = 3;
        const uint16_t extent = 4096;

        const double tolerance = baseTolerance / (z2 * extent);

        ProjectedGeometryContainer rings;

        std::vector<LonLat> points;

        for (size_t i = 0; i < segments[0].size(); ++i) { // first segment for now (no holes)
            const double constraintedLatitude = std::fmin(std::fmax(segments[0][i].latitude, -util::LATITUDE_MAX), util::LATITUDE_MAX);
            points.push_back(LonLat(segments[0][i].longitude, constraintedLatitude));
        }

        ProjectedFeatureType featureType;

        if (styleProperties.is<FillProperties>()) {
            featureType = ProjectedFeatureType::Polygon;

            if (points.front().lon != points.back().lon || points.front().lat != points.back().lat) {
                points.push_back(LonLat(points.front().lon, points.front().lat));
            }
        } else {
            featureType = ProjectedFeatureType::LineString;
        }

        ProjectedGeometryContainer ring = Convert::project(points, tolerance);

        rings.members.push_back(ring);

        std::vector<ProjectedFeature> features;

        features.push_back(Convert::create(Tags(), featureType, rings));

        shapeTilers.emplace(annotationID, std::make_unique<GeoJSONVT>(features, maxZoom, 4, 100, 10));

    } else {

        // side length of map at max zoom
        uint32_t z2 = 1 << maxZoom;

        const uint16_t extent = 4096;

        uint32_t x = 0;
        uint32_t y = 0;

        for (int8_t z = maxZoom; z >= 0; z--) {

            std::unordered_map<TileID, GeometryCollection, TileID::Hash> featureTiles;

            if (type == AnnotationType::Point) {
                auto& pp = projectedFeature[0][0];

                x = pp.x * z2;
                y = pp.y * z2;

                const Coordinate coordinate(extent * (pp.x * z2 - x), extent * (pp.y * z2 - y));

                GeometryCollection geometries = {{ {{ coordinate }} }};

                featureTiles.emplace(TileID(z, x, y, z), geometries);
            } else {
                for (size_t l = 0; l < projectedFeature.size(); ++l) {
                    for (size_t p = 0; p < projectedFeature[l].size(); ++p) {

                        auto& pp = projectedFeature[l][p];

                        x = pp.x * z2;
                        y = pp.y * z2;

                        const Coordinate coordinate(extent * (pp.x * z2 - x), extent * (pp.y * z2 - y));

                        auto tile_it = featureTiles.find(TileID(z, x, y, z));

                        if (tile_it != featureTiles.end()) {
                            GeometryCollection& geometries = featureTiles.find(TileID(z, x, y, z))->second;
                            if (geometries.size()) {
                                geometries.back().push_back(coordinate);
                            } else {
                                geometries.push_back({{ coordinate }});
                            }
                        } else {
                            GeometryCollection geometries = {{ {{ coordinate }} }};
                            featureTiles.emplace(TileID(z, x, y, z), geometries);
                        }
                    }
                }
            }

            for (auto& featureTile : featureTiles) {
                // determine feature type
                FeatureType featureType;
                if (type == AnnotationType::Point) {
                    featureType = FeatureType::Point;
                } else if (styleProperties.is<LineProperties>()) {
                    featureType = FeatureType::LineString;
                } else if (styleProperties.is<FillProperties>()) {
                    featureType = FeatureType::Polygon;
                } else {
                    throw std::runtime_error("Invalid feature type");
                }

                // create tile feature
                auto feature = std::make_shared<const LiveTileFeature>(
                    featureType,
                    featureTile.second,
                    featureProperties
                );

                // check for tile & create if necessary
                auto tile_pos = tiles.emplace(featureTile.first,
                                              std::make_pair(std::unordered_set<uint32_t>({ annotationID }),
                                                             std::make_unique<LiveTile>()));

                // check for annotation layer & create if necessary
                util::ptr<LiveTileLayer> layer;
                std::string layerID = "";
                if (type == AnnotationType::Point) {
                    layerID = PointLayerID;
                } else {
                    layerID = ShapeLayerID + "." + std::to_string(annotationID);
                }
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
                anno_it.first->second->tilePointFeatures.emplace(featureTile.first, std::weak_ptr<const LiveTileFeature>(feature));

                // track affected tile
                affectedTiles.insert(featureTile.first);
            }

            // get ready for the next-lower zoom number
            z2 /= 2;
            x /= 2;
            y /= 2;
        }
    }

    return affectedTiles;
}

std::pair<std::unordered_set<TileID, TileID::Hash>, AnnotationIDs>
AnnotationManager::addPointAnnotations(const std::vector<PointAnnotation>& points,
                                       const uint8_t maxZoom) {
    // We pre-generate tiles to contain each annotation up to the map's max zoom.
    // We do this for fast rendering without projection conversions on the fly, as well as
    // to simplify bounding box queries of annotations later. Tiles get invalidated when
    // annotations are added or removed in order to refresh the map render without
    // touching the base map underneath.

    AnnotationIDs annotationIDs;
    annotationIDs.reserve(points.size());

    std::unordered_set<TileID, TileID::Hash> affectedTiles;

    for (const PointAnnotation& point : points) {
        // projection conversion into unit space
        const auto pp = projectPoint(point.position);
        const uint32_t pointAnnotationID = nextID();

        // at render time we style the point according to its {sprite} field
        std::unordered_map<std::string, std::string> pointFeatureProperties;
        if (point.icon.length()) {
            pointFeatureProperties.emplace("sprite", point.icon);
        } else {
            pointFeatureProperties.emplace("sprite", defaultPointAnnotationSymbol);
        }

        // add individual point tile feature
        auto featureAffectedTiles = addTileFeature(
            pointAnnotationID,
            AnnotationSegments({{ point.position }}),
            std::vector<std::vector<vec2<double>>>({{ pp }}),
            AnnotationType::Point,
            {{ }},
            pointFeatureProperties,
            maxZoom
        );

        std::copy(featureAffectedTiles.begin(), featureAffectedTiles.end(), std::inserter(affectedTiles, affectedTiles.begin()));

        annotationIDs.push_back(pointAnnotationID);
    }

    // Tile:IDs that need refreshed and the annotation identifiers held onto by the client.
    return std::make_pair(affectedTiles, annotationIDs);
}

std::pair<std::unordered_set<TileID, TileID::Hash>, AnnotationIDs>
AnnotationManager::addShapeAnnotations(const std::vector<ShapeAnnotation>& shapes,
                                       const uint8_t maxZoom) {
    // We pre-generate tiles to contain each annotation up to the map's max zoom.
    // We do this for fast rendering without projection conversions on the fly, as well as
    // to simplify bounding box queries of annotations later. Tiles get invalidated when
    // annotations are added or removed in order to refresh the map render without
    // touching the base map underneath.

    AnnotationIDs annotationIDs;
    annotationIDs.reserve(shapes.size());

    std::unordered_set<TileID, TileID::Hash> affectedTiles;

    for (const ShapeAnnotation& shape : shapes) {
        const uint32_t shapeAnnotationID = nextID();

        // current shape tiles are on-the-fly, so we don't get any "affected tiles"
        // and just expire all annotation tiles for shape adds

        addTileFeature(
            shapeAnnotationID,
            shape.segments,
            {{ }},
            AnnotationType::Shape,
            shape.styleProperties,
            {{ }},
            maxZoom
        );

        annotationIDs.push_back(shapeAnnotationID);
    }

    // Tile:IDs that need refreshed and the annotation identifiers held onto by the client.
    return std::make_pair(affectedTiles, annotationIDs);
}

std::unordered_set<TileID, TileID::Hash> AnnotationManager::removeAnnotations(const AnnotationIDs& ids,
                                                                              const uint8_t maxZoom) {
    std::unordered_set<TileID, TileID::Hash> affectedTiles;

    std::vector<uint32_t> z2s;
    const uint8_t zoomCount = maxZoom + 1;
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
            // remove feature(s) from relevant tiles
            if (annotation->type == AnnotationType::Point) {
                // calculate annotation's affected tile for each zoom
                for (uint8_t z = 0; z < zoomCount; ++z) {
                    latLng = annotation->getPoint();
                    p = projectPoint(latLng);
                    x = z2s[z] * p.x;
                    y = z2s[z] * p.y;
                    TileID tid(z, x, y, z);
                    // erase annotation from tile's list
                    auto& tileAnnotations = tiles[tid].first;
                    tileAnnotations.erase(annotationID);
                    // remove annotation's features from tile
                    const auto& features_it = annotation->tilePointFeatures.find(tid);
                    if (features_it != annotation->tilePointFeatures.end()) {
                        // points share a layer; remove feature
                        auto layer = tiles[tid].second->getMutableLayer(PointLayerID);
                        layer->removeFeature(features_it->second);
                        affectedTiles.insert(tid);
                    }
                }
            } else {
                // remove shape layer from tiles if relevant
                for (auto tile_it = tiles.begin(); tile_it != tiles.end(); ++tile_it) {
                    if (tile_it->second.first.count(annotationID)) {
                        tile_it->second.second->removeLayer(ShapeLayerID + "." + std::to_string(annotationID));
                        affectedTiles.insert(tile_it->first);
                    }
                }

                // clear shape from render order
                auto shape_it = std::find(orderedShapeAnnotations.begin(), orderedShapeAnnotations.end(), annotationID);
                orderedShapeAnnotations.erase(shape_it);

                // clear shape tiler
                shapeTilers.erase(annotationID);
            }

            annotations.erase(annotationID);
        }
    }

    // TileIDs for tiles that need refreshed.
    return affectedTiles;
}

const StyleProperties AnnotationManager::getAnnotationStyleProperties(uint32_t annotationID) const {
    auto anno_it = annotations.find(annotationID);
    assert(anno_it != annotations.end());

    return anno_it->second->styleProperties;
}

AnnotationIDs AnnotationManager::getAnnotationsInBounds(const LatLngBounds& queryBounds,
                                                        const uint8_t maxZoom,
                                                        const AnnotationType& type) const {
    const uint8_t z = maxZoom;
    const uint32_t z2 = 1 << z;
    const vec2<double> swPoint = projectPoint(queryBounds.sw);
    const vec2<double> nePoint = projectPoint(queryBounds.ne);

    // tiles number y from top down
    const TileID nwTile(z, swPoint.x * z2, nePoint.y * z2, z);
    const TileID seTile(z, nePoint.x * z2, swPoint.y * z2, z);

    std::unordered_set<uint32_t> matchingAnnotations;

    for (auto& tile : tiles) {
        TileID id = tile.first;
        if (id.z == z) {
            if (id.x >= nwTile.x && id.x <= seTile.x && id.y >= nwTile.y && id.y <= seTile.y) {
                if (id.x > nwTile.x && id.x < seTile.x && id.y > nwTile.y && id.y < seTile.y) {
                    // Trivial accept; this tile is completely inside the query bounds, so
                    // we'll return all of its annotations that match type (if specified).
                    if (type != AnnotationType::Any) {
                        std::copy_if(tile.second.first.begin(), tile.second.first.end(),
                                     std::inserter(matchingAnnotations, matchingAnnotations.begin()),
                                     [&](const uint32_t annotationID) -> bool {
                            const auto it = annotations.find(annotationID);
                            if (it != annotations.end()) {
                                return (it->second->type == type);
                            } else {
                                return false;
                            }
                        });
                    } else {
                        std::copy(tile.second.first.begin(), tile.second.first.end(),
                                  std::inserter(matchingAnnotations, matchingAnnotations.begin()));
                    }
                } else {
                    // This tile is intersected by the query bounds. We need to check the
                    // tile's annotations' bounding boxes individually.
                    std::copy_if(tile.second.first.begin(), tile.second.first.end(),
                                 std::inserter(matchingAnnotations, matchingAnnotations.begin()),
                                 [&](const uint32_t annotationID) -> bool {
                        const auto it = annotations.find(annotationID);
                        if (it != annotations.end()) {
                            // check type
                            if (type != AnnotationType::Any && it->second->type != type) {
                                return false;
                            }

                            // check bounds
                            if (it->second->type == AnnotationType::Point) {
                                return queryBounds.contains(it->second->getPoint());
                            } else if (it->second->type == AnnotationType::Shape) {
                                return queryBounds.intersects(it->second->getBounds());
                            }
                        }
                        return false;
                    });
                }
            }
        }
    }

    return AnnotationIDs(matchingAnnotations.begin(), matchingAnnotations.end());
}

LatLngBounds AnnotationManager::getBoundsForAnnotations(const AnnotationIDs& ids) const {
    LatLngBounds bounds;
    for (auto id : ids) {
        const auto annotation_it = annotations.find(id);
        if (annotation_it != annotations.end()) {
            bounds.extend(annotation_it->second->getBounds());
        }
    }

    return bounds;
}

const LiveTile* AnnotationManager::getTile(const TileID& id) {
    // look up any existing annotation tile
    LiveTile *renderTile = nullptr;
    const auto tile_lookup_it = tiles.find(id);
    if (tile_lookup_it != tiles.end()) {
        // it exists and may have annotations already
        renderTile = tile_lookup_it->second.second.get();
    } else if (orderedShapeAnnotations.size()) {
        // it needs created, but only for on-demand shapes
        renderTile = tiles.emplace(id,
            std::make_pair(std::unordered_set<uint32_t>(), std::make_unique<LiveTile>())
        ).first->second.second.get();
    }

    if (renderTile != nullptr && orderedShapeAnnotations.size()) {

        // create shape tile layers from GeoJSONVT queries
        for (auto& tiler_it : shapeTilers) {
            const auto annotationID = tiler_it.first;
            const std::string layerID = ShapeLayerID + "." + std::to_string(annotationID);

            // check for existing render layer
            auto renderLayer = renderTile->getMutableLayer(layerID);

            if (renderLayer == nullptr) {
                // we might need to create a tile layer for this shape
                const auto& shapeTile = tiler_it.second->getTile(id.z, id.x, id.y);

                if (shapeTile) {

                    // shape exists on this tile; let's make a layer
                    renderLayer = std::make_shared<LiveTileLayer>();

                    // convert the features and add to render layer
                    for (auto& shapeFeature : shapeTile.features) {

                        using namespace mapbox::util::geojsonvt;

                        FeatureType renderType = FeatureType::Unknown;

                        if (shapeFeature.type == TileFeatureType::LineString) {
                            renderType = FeatureType::LineString;
                        } else if (shapeFeature.type == TileFeatureType::Polygon) {
                            renderType = FeatureType::Polygon;
                        }

                        assert(renderType != FeatureType::Unknown);

                        GeometryCollection renderGeometry;

                        for (auto& shapeGeometry : shapeFeature.geometry) {

                            std::vector<Coordinate> renderLine;

                            auto& shapeRing = shapeGeometry.get<TileRing>();

                            for (auto& shapePoint : shapeRing.points) {
                                renderLine.emplace_back(shapePoint.x, shapePoint.y);
                            }

                            renderGeometry.push_back(renderLine);
                        }

                        auto renderFeature = std::make_shared<LiveTileFeature>(renderType, renderGeometry);

                        renderLayer->addFeature(renderFeature);
                    }

                    // move the layer to the render tile
                    renderTile->addLayer(layerID, renderLayer);

                    // associate the annotation with the tile
                    auto tile_update_it = tiles.find(id);
                    assert(tile_update_it != tiles.end());
                    tile_update_it->second.first.insert(annotationID);
                }
            }
        }
    }

    return renderTile;
}

const std::string AnnotationManager::PointLayerID = "com.mapbox.annotations.points";
const std::string AnnotationManager::ShapeLayerID = "com.mapbox.annotations.shape";

}
