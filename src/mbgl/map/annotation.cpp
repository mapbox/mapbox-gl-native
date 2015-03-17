#include <mbgl/map/annotation.hpp>
#include <mbgl/util/ptr.hpp>

#include <algorithm>
#include <memory>

using namespace mbgl;

Annotation::Annotation(AnnotationType type_, std::vector<AnnotationSegment> geometry_)
    : type(type_),
      geometry(geometry_) {
    if (type == AnnotationType::Point) {
        bbox = BoundingBox(getPoint(), getPoint());
    } else {
        LatLng sw, ne;
        for (auto segment : geometry) {
            for (auto point : segment) {
                if (point.latitude < sw.latitude) sw.latitude = point.latitude;
                if (point.latitude > ne.latitude) ne.latitude = point.latitude;
                if (point.longitude < sw.longitude) sw.longitude = point.longitude;
                if (point.longitude > ne.longitude) ne.longitude = point.longitude;
            }
        }
        bbox = BoundingBox(sw, ne);
    }
}

AnnotationManager::AnnotationManager(Map& map_)
    : map(map_),
    nullTile(util::make_unique<LiveTile>()) {}

vec2<double> AnnotationManager::projectPoint(LatLng& point) {
    double sine = std::sin(point.latitude * M_PI / 180);
    double x = point.longitude / 360 + 0.5;
    double y = 0.5 - 0.25 * std::log((1 + sine) / (1 - sine)) / M_PI;
    return vec2<double>(x, y);
}

std::pair<std::vector<Tile::ID>, std::vector<uint32_t>> AnnotationManager::addPointAnnotations(std::vector<LatLng> points, std::vector<std::string>& symbols) {

    uint16_t extent = 4096;

    std::vector<uint32_t> annotationIDs(points.size());
    std::vector<Tile::ID> affectedTiles;

    for (uint32_t i = 0; i < points.size(); ++i) {
        uint32_t annotationID = nextID();

        auto anno_it = annotations.emplace(annotationID, util::make_unique<Annotation>(AnnotationType::Point, std::vector<AnnotationSegment>({{ points[i] }})));

        uint8_t maxZoom = map.getMaxZoom();

        uint32_t z2 = 1 << maxZoom;

        vec2<double> p = projectPoint(points[i]);

        uint32_t x = p.x * z2;
        uint32_t y = p.y * z2;

        for (int8_t z = maxZoom; z >= 0; z--) {
            affectedTiles.emplace_back(z, x, y);
            Tile::ID tileID = affectedTiles.back();

            Coordinate coordinate(extent * (p.x * z2 - x), extent * (p.y * z2 - y));

            GeometryCollection geometries({{ {{ coordinate }} }});

            std::map<std::string, std::string> properties = {{ "sprite", (symbols[i].length() ? symbols[i] : defaultPointAnnotationSymbol) }};

            auto feature = std::make_shared<const LiveTileFeature>(FeatureType::Point,
                                                                   geometries,
                                                                   properties);

            auto tile_it = annotationTiles.find(tileID);
            if (tile_it != annotationTiles.end()) {
                // get point layer & add feature
                auto layer = tile_it->second.second->getMutableLayer(util::ANNOTATIONS_POINTS_LAYER_ID);
                layer->addFeature(feature);
                // record annotation association with tile
                tile_it->second.first.push_back(annotationID);
            } else {
                // create point layer & add feature
                util::ptr<LiveTileLayer> layer = std::make_shared<LiveTileLayer>();
                layer->addFeature(feature);
                // create tile & record annotation association
                auto tile_pos = annotationTiles.emplace(tileID, std::make_pair(std::vector<uint32_t>({ annotationID }), util::make_unique<LiveTile>()));
                // add point layer to tile
                tile_pos.first->second.second->addLayer(util::ANNOTATIONS_POINTS_LAYER_ID, layer);
            }

            // record annotation association with tile feature
            anno_it.first->second->tileFeatures.emplace(tileID, std::vector<std::weak_ptr<const LiveTileFeature>>({ feature }));

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
                auto features_it = annotation->tileFeatures.find(tile_it.first);
                if (features_it != annotation->tileFeatures.end()) {
                    auto layer = tile_it.second.second->getLayer(util::ANNOTATIONS_POINTS_LAYER_ID);
                    auto liveLayer = std::static_pointer_cast<LiveTileLayer>(layer);
                    auto& features = features_it->second;
                    liveLayer->removeFeature(features[0]);
                    affectedTiles.push_back(tile_it.first);
                }
            }
            annotations.erase(annotationID);
        }
    }

    return affectedTiles;
}

std::vector<uint32_t> AnnotationManager::getAnnotationsInBoundingBox(BoundingBox queryBbox) const {
    uint8_t z = map.getMaxZoom();
    uint32_t z2 = 1 << z;
    vec2<double> swPoint = projectPoint(queryBbox.sw);
    vec2<double> nePoint = projectPoint(queryBbox.ne);

    // tiles number y from top down
    Tile::ID nwTile(z, swPoint.x * z2, nePoint.y * z2);
    Tile::ID seTile(z, nePoint.x * z2, swPoint.y * z2);

    std::vector<uint32_t> matchingAnnotations;

    for (auto& tile : annotationTiles) {
        Tile::ID id = tile.first;
        if (id.z == z) {
            if (id.x >= nwTile.x && id.x <= seTile.x && id.y >= nwTile.y && id.y <= seTile.y) {
                if (id.x > nwTile.x && id.x < seTile.x && id.y > nwTile.y && id.y < seTile.y) {
                    // trivial accept; grab all of the tile's annotations
                    std::copy(tile.second.first.begin(), tile.second.first.end(), std::back_inserter(matchingAnnotations));
                } else {
                    // check tile's annotations' bounding boxes
                    std::copy_if(tile.second.first.begin(), tile.second.first.end(),
                                 std::back_inserter(matchingAnnotations), [&](const uint32_t annotationID) -> bool {
                        printf("checking annotation %i\n", annotationID);
                        BoundingBox annoBbox = this->annotations.find(annotationID)->second->getBoundingBox();
                        return (annoBbox.sw.latitude >= queryBbox.sw.latitude &&
                                annoBbox.ne.latitude <= queryBbox.ne.latitude &&
                                annoBbox.sw.longitude >= queryBbox.sw.longitude &&
                                annoBbox.ne.longitude <= queryBbox.ne.longitude);
                    });
                }
            }
        }
    }

    return matchingAnnotations;
}

BoundingBox AnnotationManager::getBoundingBoxForAnnotations(std::vector<uint32_t> ids) const {
    LatLng sw, ne;
    for (auto id : ids) {
        auto annotation_it = annotations.find(id);
        if (annotation_it != annotations.end()) {
            auto bbox = annotation_it->second->getBoundingBox();
            if (bbox.sw.latitude < sw.latitude) sw.latitude = bbox.sw.latitude;
            if (bbox.ne.latitude > ne.latitude) ne.latitude = bbox.ne.latitude;
            if (bbox.sw.longitude < sw.longitude) sw.longitude = bbox.sw.longitude;
            if (bbox.ne.longitude > ne.longitude) ne.longitude = bbox.ne.longitude;
        }
    }

    return BoundingBox(sw, ne);
}

const std::unique_ptr<LiveTile>& AnnotationManager::getTile(Tile::ID const& id) {
    std::lock_guard<std::mutex> lock(mtx);

    auto tile_it = annotationTiles.find(id);
    if (tile_it != annotationTiles.end()) {
        return tile_it->second.second;
    }
    return nullTile;
}
