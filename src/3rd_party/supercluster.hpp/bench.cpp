#include <mapbox/geometry/feature.hpp>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#define DEBUG_TIMER true

#include <supercluster.hpp>

#include <cassert>
#include <cstdio>
#include <iostream>
#include <vector>

int main() {
    std::FILE *fp = std::fopen("../supercluster/tmp/trees-na2.json", "r");
    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));

    mapbox::supercluster::Timer timer;

    rapidjson::Document d;
    d.ParseStream(is);
    timer("parse JSON");

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
    timer("convert to geometry.hpp");

    mapbox::supercluster::Options options;
    options.radius = 75;
    mapbox::supercluster::Supercluster index(features, options);

    timer("total supercluster time");

    mapbox::geometry::feature_collection<std::int16_t> tile = index.getTile(0, 0, 0);
    timer("query zero tile");
    std::cerr << tile.size() << " features in tile 0-0-0:\n";

    for (auto &f : tile) {
        const auto itr = f.properties.find("cluster");
        if (itr != f.properties.end() && itr->second.get<bool>()) {
            std::cerr << "cluster: " << f.properties["point_count"].get<std::uint64_t>() << "\n";
        } else {
            std::cerr << "point\n";
        }
    }
}
