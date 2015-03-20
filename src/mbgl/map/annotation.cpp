#include <mbgl/map/annotation.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/std.hpp>

#include <algorithm>
#include <memory>

using namespace mbgl;

Annotation::Annotation(AnnotationType type_, std::vector<AnnotationSegment> geometry_)
    : type(type_),
      geometry(geometry_) {
    if (type == AnnotationType::Point) {
        bounds = LatLngBounds(getPoint(), getPoint());
    } else {
        for (auto segment : geometry) {
            for (auto point : segment) {
                bounds.extend(point);
            }
        }
    }
}

LatLng Annotation::getPoint() const {
    return geometry[0][0];
}

AnnotationManager::AnnotationManager()
    : nullTile(util::make_unique<LiveTile>()) {}

vec2<double> AnnotationManager::projectPoint(LatLng& point) {
    double sine = std::sin(point.latitude * M_PI / 180);
    double x = point.longitude / 360 + 0.5;
    double y = 0.5 - 0.25 * std::log((1 + sine) / (1 - sine)) / M_PI;
    return vec2<double>(x, y);
}

std::pair<std::vector<Tile::ID>, std::vector<uint32_t>> AnnotationManager::addPointAnnotations(std::vector<LatLng> points, std::vector<std::string>& symbols, const Map& map) {
    //
    // We pre-generate tiles to contain each annotation up to the map's max zoom.
    // We do this for fast rendering without projection conversions on the fly, as well as
    // to simplify bounding box queries of annotations later. Tiles get invalidated when
    // annotations are added or removed in order to refresh the map render without
    // touching the base map underneath.
    //
    uint16_t extent = 4096;

    std::vector<uint32_t> annotationIDs;
    annotationIDs.reserve(points.size());
    std::vector<Tile::ID> affectedTiles;

    for (uint32_t i = 0; i < points.size(); ++i) {
        uint32_t annotationID = nextID();

        // track the annotation global ID and its geometry
        auto anno_it = annotations.emplace(annotationID, util::make_unique<Annotation>(AnnotationType::Point, std::vector<AnnotationSegment>({{ points[i] }})));

        uint8_t maxZoom = map.getMaxZoom();

        // side length of map at this zoom
        uint32_t z2 = 1 << maxZoom;

        // projection conversion into unit space
        vec2<double> p = projectPoint(points[i]);

        uint32_t x = p.x * z2;
        uint32_t y = p.y * z2;

        for (int8_t z = maxZoom; z >= 0; z--) {
            affectedTiles.emplace_back(z, x, y);
            Tile::ID tileID = affectedTiles.back();

            // calculate tile coordinate
            Coordinate coordinate(extent * (p.x * z2 - x), extent * (p.y * z2 - y));

            GeometryCollection geometries({{ {{ coordinate }} }});

            // at render time we style the annotation according to its {sprite} field
            std::map<std::string, std::string> properties = {{ "sprite", (symbols[i].length() ? symbols[i] : defaultPointAnnotationSymbol) }};

            auto feature = std::make_shared<const LiveTileFeature>(FeatureType::Point,
                                                                   geometries,
                                                                   properties);

            auto tile_it = annotationTiles.find(tileID);
            if (tile_it != annotationTiles.end()) {
                //
                // We have this tile created already. Add this feature to it.
                //

                // get point layer & add feature
                auto layer = tile_it->second.second->getMutableLayer(util::ANNOTATIONS_POINTS_LAYER_ID);
                layer->addFeature(feature);
                // record annotation association with tile
                tile_it->second.first.push_back(annotationID);
            } else {
                //
                // We need to create a new tile for this feature.
                //

                // create point layer & add feature
                util::ptr<LiveTileLayer> layer = std::make_shared<LiveTileLayer>();
                layer->addFeature(feature);
                // create tile & record annotation association
                auto tile_pos = annotationTiles.emplace(tileID, std::make_pair(std::vector<uint32_t>({ annotationID }), util::make_unique<LiveTile>()));
                // add point layer to tile
                tile_pos.first->second.second->addLayer(util::ANNOTATIONS_POINTS_LAYER_ID, layer);
            }

            // Record annotation association with tile and tile feature. This is used to determine stale tiles,
            // as well as to remove the feature from the tile upon annotation deletion.
            anno_it.first->second->tileFeatures.emplace(tileID, std::vector<std::weak_ptr<const LiveTileFeature>>({ feature }));

            // get ready for the next-lower zoom number
            z2 /= 2;
            x /= 2;
            y /= 2;
        }

        annotationIDs.push_back(annotationID);
    }

    return std::make_pair(affectedTiles, annotationIDs);
}

std::vector<Tile::ID> AnnotationManager::removeAnnotations(std::vector<uint32_t> ids) {
    std::vector<Tile::ID> affectedTiles;

    for (auto& annotationID : ids) {
        auto annotation_it = annotations.find(annotationID);
        if (annotation_it != annotations.end()) {
            auto& annotation = annotation_it->second;
            for (auto& tile_it : annotationTiles) {
                // clean up tile -> annotation mapping used for bounds queries
                auto& tileAnnotations = tile_it.second.first;
                util::erase_if(tileAnnotations, tileAnnotations.begin(),
                               tileAnnotations.end(), [&](const uint32_t annotationID_) -> bool {
                                   return (annotationID_ == annotationID);
                               });
                // remove annotation feature from tile layer
                auto features_it = annotation->tileFeatures.find(tile_it.first);
                if (features_it != annotation->tileFeatures.end()) {
                    auto layer = tile_it.second.second->getMutableLayer(util::ANNOTATIONS_POINTS_LAYER_ID);
                    auto& features = features_it->second;
                    layer->removeFeature(features[0]);
                    affectedTiles.push_back(tile_it.first);
                }
            }
            annotations.erase(annotationID);
        }
    }

    return affectedTiles;
}

std::vector<uint32_t> AnnotationManager::getAnnotationsInBounds(LatLngBounds queryBounds, const Map& map) const {
    uint8_t z = map.getMaxZoom();
    uint32_t z2 = 1 << z;
    vec2<double> swPoint = projectPoint(queryBounds.sw);
    vec2<double> nePoint = projectPoint(queryBounds.ne);

    // tiles number y from top down
    Tile::ID nwTile(z, swPoint.x * z2, nePoint.y * z2);
    Tile::ID seTile(z, nePoint.x * z2, swPoint.y * z2);

    std::vector<uint32_t> matchingAnnotations;

    for (auto& tile : annotationTiles) {
        Tile::ID id = tile.first;
        if (id.z == z) {
            if (id.x >= nwTile.x && id.x <= seTile.x && id.y >= nwTile.y && id.y <= seTile.y) {
                if (id.x > nwTile.x && id.x < seTile.x && id.y > nwTile.y && id.y < seTile.y) {
                    // Trivial accept; this tile is completely inside the query bounds, so
                    // we'll return all of its annotations.
                    std::copy(tile.second.first.begin(), tile.second.first.end(), std::back_inserter(matchingAnnotations));
                } else {
                    // This tile is intersected by the query bounds. We need to check the
                    // tile's annotations' bounding boxes individually. 
                    std::copy_if(tile.second.first.begin(), tile.second.first.end(),
                                 std::back_inserter(matchingAnnotations), [&](const uint32_t annotationID) -> bool {
                        LatLngBounds annoBounds = this->annotations.find(annotationID)->second->getBounds();
                        return (annoBounds.sw.latitude >= queryBounds.sw.latitude &&
                                annoBounds.ne.latitude <= queryBounds.ne.latitude &&
                                annoBounds.sw.longitude >= queryBounds.sw.longitude &&
                                annoBounds.ne.longitude <= queryBounds.ne.longitude);
                    });
                }
            }
        }
    }

    return matchingAnnotations;
}

LatLngBounds AnnotationManager::getBoundsForAnnotations(std::vector<uint32_t> ids) const {
    LatLngBounds bounds;
    for (auto id : ids) {
        auto annotation_it = annotations.find(id);
        if (annotation_it != annotations.end()) {
            bounds.extend(annotation_it->second->getPoint());
        }
    }

    return bounds;
}

const std::unique_ptr<LiveTile>& AnnotationManager::getTile(Tile::ID const& id) {
    std::lock_guard<std::mutex> lock(mtx);

    auto tile_it = annotationTiles.find(id);
    if (tile_it != annotationTiles.end()) {
        return tile_it->second.second;
    }
    return nullTile;
}
