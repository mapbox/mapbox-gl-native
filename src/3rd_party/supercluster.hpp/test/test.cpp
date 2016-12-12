#include <mapbox/geometry/feature.hpp>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include <supercluster.hpp>

#include <cassert>
#include <cstdio>
#include <iostream>
#include <vector>

int main() {
    std::FILE *fp = std::fopen("test/fixtures/places.json", "r");
    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));

    rapidjson::Document d;
    d.ParseStream(is);

    const auto &json_features = d["features"];

    mapbox::geometry::feature_collection<double> features;
    features.reserve(json_features.Size());

    for (auto itr = json_features.Begin(); itr != json_features.End(); ++itr) {
        const auto &json_coords = (*itr)["geometry"]["coordinates"];
        const auto lng = json_coords[0].GetDouble();
        const auto lat = json_coords[1].GetDouble();
        mapbox::geometry::point<double> point(lng, lat);
        mapbox::geometry::feature<double> feature{ point };
        features.push_back(feature);
    }

    mapbox::supercluster::Options options;
    mapbox::supercluster::Supercluster index(features, options);

    mapbox::geometry::feature_collection<std::int16_t> tile = index.getTile(0, 0, 0);
    assert(tile.size() == 39);

    std::uint64_t num_points = 0;

    for (auto &f : tile) {
        const auto itr = f.properties.find("cluster");
        if (itr != f.properties.end() && itr->second.get<bool>()) {
            num_points += f.properties["point_count"].get<std::uint64_t>();
        } else {
            num_points += 1;
        }
    }

    assert(num_points == 196);
}
