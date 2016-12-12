#include <mapbox/polylabel.hpp>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace mapbox;

// Use the CrtAllocator, because the MemoryPoolAllocator is broken on ARM
// https://github.com/miloyip/rapidjson/issues/200, 301, 388
using rapidjson_allocator = rapidjson::CrtAllocator;
using rapidjson_document = rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson_allocator>;
using rapidjson_value = rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson_allocator>;

geometry::polygon<double> fixture(const std::string& path) {
    std::ifstream t(path.c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();

    rapidjson_document d;
    d.Parse(buffer.str().c_str());

    geometry::polygon<double> result;

    for (const auto& ringArray : d.GetArray()) {
        geometry::linear_ring<double> ring;
        for (const auto& point : ringArray.GetArray()) {
            ring.push_back({
                point[0].GetDouble(),
                point[1].GetDouble()
            });
        }
        result.push_back(std::move(ring));
    }

    return result;
}

int main() {
    geometry::polygon<double> water1 = fixture("./test/fixtures/water1.json");
    geometry::polygon<double> water2 = fixture("./test/fixtures/water2.json");

    // finds pole of inaccessibility for water1 and precision 1
    assert(polylabel(water1, 1.0) == geometry::point<double>(3865.85009765625, 2124.87841796875));

    // finds pole of inaccessibility for water1 and precision 50
    assert(polylabel(water1, 50.0) == geometry::point<double>(3854.296875, 2123.828125));

    // finds pole of inaccessibility for water2 and default precision 1
    assert(polylabel(water2) == geometry::point<double>(3263.5, 3263.5));

    // works on degenerate polygons
    assert(polylabel(geometry::polygon<double>({{{0, 0}, {1, 0}, {2, 0}, {0, 0}}}))
        == geometry::point<double>(0, 0));
    assert(polylabel(geometry::polygon<double>({{{0, 0}, {1, 0}, {1, 1}, {1, 0}, {0, 0}}}))
        == geometry::point<double>(0, 0));

    return 0;
}
