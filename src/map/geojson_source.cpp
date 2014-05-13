#include <llmr/map/geojson_source.hpp>
#include <llmr/map/map.hpp>
#include <llmr/map/transform.hpp>
#include <llmr/map/vector_tile_data.hpp>
#include <llmr/renderer/line_bucket.hpp>
#include <llmr/util/std.hpp>
#include <llmr/util/string.hpp>

#include <cfloat>
#include <stdexcept>

using namespace llmr;

GeoJSONSource::GeoJSONSource(Map &map, Painter &painter, const char *url,
                             std::vector<uint32_t> zooms, uint32_t tile_size,
                             uint32_t min_zoom, uint32_t max_zoom, bool enabled) :
    Source(map, painter, url, Type::geojson, zooms, tile_size, min_zoom, max_zoom, enabled),
    all_tiles(),
    transforms() {
    platform::request_http(url, [zooms, tile_size, &map, this](platform::Response *res) {
        if (res->code == 200) {
            // parse JSON
            rapidjson::Document d = parseJSON(res->body.c_str());
            // build transforms
            for (uint8_t z = 0; z < zooms.size(); z++) {
                std::shared_ptr<Transform> t = std::make_shared<Transform>();
                transforms.push_back(t);
                t->resize(tile_size, tile_size, map.getState().getPixelRatio(), tile_size, tile_size);
                t->setZoom(zooms[z]);
            };
            // tile the features
            tile_GeoJSON(d["routes"][(rapidjson::SizeType)0]["geometry"]);
            // re-render
            map.update();
        } else {
            fprintf(stderr, "failed to load GeoJSON\n");
        }
    }, map.getLoop());
}

rapidjson::Document GeoJSONSource::parseJSON(const char *data) {
    rapidjson::Document d;
    d.Parse<0>(data);

    if (!d.IsObject() ||
        !d.HasMember("routes") ||
        !d["routes"].IsArray() ||
        !d["routes"][(rapidjson::SizeType)0].HasMember("geometry")) {
        throw std::runtime_error("malformed GeoJSON");
    }

    return d;
}

void GeoJSONSource::tile_GeoJSON(JSVal& geojson) {
    for (uint8_t k = 0; k < transforms.size(); k++) {
        std::shared_ptr<Transform>& transform = transforms[k];

        // We should check for Feature/FeatureCollection; assume
        // single Feature for now.
//        if (geojson.type === 'FeatureCollection') {
//            for (var i = 0; i < geojson.features.length; i++) {
//                this._tileFeature(geojson.features[i], transform);
//            }
//
//        } else if (geojson.type === 'Feature') {
//            this._tileFeature(geojson, transform);
//
//        } else {
//            throw('Unrecognized geojson type');
//        }

        tile_feature(geojson, transform);
    }

    // Update 'tiles' with renderable tile structures from the
    // parsed feature tiles.
    for (auto it = all_tiles.begin(); it != all_tiles.end(); it++) {
        Tile::ID id = it->first;
        GeoJSONTile feature_tile = it->second;

        auto pos = tiles.emplace(id, std::make_unique<Tile>(id));
        Tile& new_tile = *pos.first->second;

        const Tile::ID normalized_id = id.normalized();

        std::shared_ptr<VectorTileData> new_tile_data = std::make_shared<VectorTileData>(normalized_id, map, "");
        new_tile.data = new_tile_data;

        // 1. assume route bucket (for now)
        const BucketDescription& bucket_desc = map.getStyle().buckets.find("route")->second;

        // 2. create line bucket
        std::unique_ptr<LineBucket> bucket = std::make_unique<LineBucket>(new_tile_data->lineVertexBuffer, new_tile_data->triangleElementsBuffer, new_tile_data->pointElementsBuffer, bucket_desc);

        // 3. add bucket features
        for (Segment segment : feature_tile) {
            bucket->addGeometry(segment);
        }

        // 4. add to parsed tiles
        new_tile_data->buckets["route"] = std::move(bucket);
        tile_data.emplace(id, new_tile_data);
        new_tile_data->state = TileData::State::parsed;
    }

    all_tiles.clear();
    transforms.clear();
}

void GeoJSONSource::tile_feature(JSVal& feature, std::shared_ptr<Transform>& transform) {
    JSVal coords = feature["coordinates"];
    JSVal geom_type = feature["type"];

    std::map<Tile::ID, GeoJSONTile> tiled;

    if (!strcmp(geom_type.GetString(), "Point")) {
        fprintf(stderr, "TODO: Point GeoJSON\n");
//            tiled = this._tileLineString([coords], transform);
    } else if (!strcmp(geom_type.GetString(), "LineString") || strcmp(geom_type.GetString(), "MultiPoint")) {
        tiled = tile_line_string(coords, transform);
    } else if (!strcmp(geom_type.GetString(), "Polygon") || strcmp(geom_type.GetString(), "MultiLineString")) {
        fprintf(stderr, "TODO: Polygon GeoJSON\n");
//            tiled = {};
//            for (var i = 0; i < coords.length; i++) {
//                var tiled_ = this._tileLineString(coords[i], transform, type === 'Polygon');
//                for (var tileID in tiled_) {
//                    if (!tiled[tileID]) tiled[tileID] = [];
//                    tiled[tileID] = (tiled[tileID] || []).concat(tiled_[tileID]);
//                }
//            }
    } else if (!strcmp(geom_type.GetString(), "MultiPolygon")) {
        fprintf(stderr, "TODO: MultiPolygon GeoJSON\n");
    } else {
        throw std::runtime_error("unrecognized geometry type");
    }

    for (auto it = tiled.begin(); it != tiled.end(); it++) {
        all_tiles[it->first] = it->second;
    }
}

std::map<Tile::ID, GeoJSONTile> GeoJSONSource::tile_line_string(JSVal coords, std::shared_ptr<Transform>& transform, bool rejoin) {
    JSVal c = coords[(rapidjson::SizeType)0];
    double c_lon = c[(rapidjson::SizeType)0].GetDouble();
    double c_lat = c[(rapidjson::SizeType)1].GetDouble();
    Transform::location_coordinate coord = transform->getLocationCoordinate(c_lon, c_lat);
    Transform::location_coordinate prev_coord;

    std::map<Tile::ID, GeoJSONTile> feature_tiles;

    for (uint32_t i = 0; i < coords.Size(); i++) {
        prev_coord = coord;
        JSVal c = coords[i];
        double c_lon = c[(rapidjson::SizeType)0].GetDouble();
        double c_lat = c[(rapidjson::SizeType)1].GetDouble();
        coord = transform->getLocationCoordinate(c_lon, c_lat);

        double dx = (coord.column - prev_coord.column ? coord.column - prev_coord.column : DBL_MIN);
        double dy = (coord.row - prev_coord.row ? coord.row - prev_coord.row : DBL_MIN);
        double dir_x = dx / fabs(dx);
        double dir_y = dy / fabs(dy);

        // Find the rectangular bounding box, in tiles, of the polygon
        double start_tile_x = floor(prev_coord.column - dir_x * padding);
        double end_tile_x = floor(coord.column + dir_x * padding);
        double start_tile_y = floor(prev_coord.row - dir_y * padding);
        double end_tile_y = floor(coord.row + dir_y * padding);

        // Iterate over all tiles the segment might intersect
        // and split the segment across those tiles
        for (int32_t x = start_tile_x; (x - end_tile_x) * dir_x <= 0; x += dir_x) {
            double left_x = (x - padding - prev_coord.column) / dx;
            double right_x = (x + 1 + padding - prev_coord.column) / dx;

            for (int32_t y = start_tile_y; (y - end_tile_y) * dir_y <= 0; y += dir_y) {
                double top_y = (y - padding - prev_coord.row) / dy;
                double bottom_y = (y + 1 + padding - prev_coord.row) / dy;

                // fraction of the distance along the segment at which the segment
                // enters or exits the tile
                double enter = fmax(fmin(left_x, right_x), fmin(top_y, bottom_y));
                double exit = fmin(fmax(left_x, right_x), fmax(top_y, bottom_y));

                Tile::ID id(floor(transform->getZoom()), x, y);
                GeoJSONTile& tile = feature_tiles[id];
                Coordinate point;

                // segments starts outside the tile, add entry point
                if (0 <= enter && enter < 1) {
                    point.x = ((prev_coord.column + enter * dx) - x) * tile_extent;
                    point.y = ((prev_coord.row + enter * dy) - y) * tile_extent;

                    // point.continues = true;

                    Segment segment;
                    segment.push_back(point);
                    tile.push_back(segment);
                }

                // segments ends outside the tile, add exit point
                if (0 <= exit && exit < 1) {
                    point.x = ((prev_coord.column + exit * dx) - x) * tile_extent;
                    point.y = ((prev_coord.row + exit * dy) - y) * tile_extent;

                    // point.continues = true;

                    tile.back().push_back(point);

                // add the point itself
                } else {
                    point.x = (coord.column - x) * tile_extent;
                    point.y = (coord.row - y) * tile_extent;

                    if (!tile.size()) {
                        Segment segment;
                        segment.push_back(point);
                        tile.push_back(segment);
                    } else {
                        tile.back().push_back(point);
                    }
                }
            }
        }
    }

    if (rejoin) {
        // TODO

        // reassemble the disconnected segments into a linestring
        // sections of the linestring outside the tile are replaced with segments
        // that follow the tile's edge
    }

    return feature_tiles;
}
