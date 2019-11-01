#include <mbgl/test/util.hpp>

#include <mbgl/geometry/line_atlas.hpp>

#include <random>

using namespace mbgl;

TEST(LineAtlas, Random) {
    std::mt19937 generator(42); // Use the same seed for reproducible tests.
    std::uniform_int_distribution<size_t> countDistribution(0, 12);
    std::uniform_int_distribution<size_t> capStyleDistribution(0, 1);
    std::normal_distribution<float> lengthDistribution(3, 5);

    for (size_t it = 0; it < 100; it++) {
        LineAtlas atlas;
        std::vector<float> dasharray;
        dasharray.reserve(8);
        for (size_t j = 0; j < 100; j++) {
            dasharray.resize(0);
            const size_t count = countDistribution(generator);
            for (size_t i = 0; i < count; i++) {
                dasharray.push_back(lengthDistribution(generator));
            }
            const LinePatternCap patternCap =
                capStyleDistribution(generator) > 0 ? LinePatternCap::Round : LinePatternCap::Square;

            atlas.getDashPatternTexture(dasharray, dasharray, patternCap);
        }
    }
}
