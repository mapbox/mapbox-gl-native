#include <mbgl/text/cross_tile_symbol_index.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/test/util.hpp>

using namespace mbgl;

SymbolInstance makeSymbolInstance(float x, float y, std::u16string key) {
    GeometryCoordinates line;
    ImageMap imageMap;
    const ShapedTextOrientations shaping{};
    style::SymbolLayoutProperties::Evaluated layout_;
    IndexedSubfeature subfeature(0, "", "", 0);
    Anchor anchor(x, y, 0, 0);
    std::array<float, 2> textOffset{{0.0f, 0.0f}};
    std::array<float, 2> iconOffset{{0.0f, 0.0f}};
    std::array<float, 2> variableTextOffset{{0.0f, 0.0f}};
    style::SymbolPlacementType placementType = style::SymbolPlacementType::Point;

    auto sharedData = std::make_shared<SymbolInstanceSharedData>(std::move(line),
                                                                 shaping,
                                                                 nullopt,
                                                                 nullopt,
                                                                 layout_,
                                                                 placementType,
                                                                 textOffset,
                                                                 imageMap,
                                                                 0,
                                                                 SymbolContent::IconSDF,
                                                                 false,
                                                                 false);
    return SymbolInstance(anchor, std::move(sharedData), shaping, nullopt, nullopt, 0, 0, placementType, textOffset, 0, 0, iconOffset, subfeature, 0, 0, key, 0.0f, 0.0f, 0.0f, variableTextOffset, false);
}


TEST(CrossTileSymbolLayerIndex, addBucket) {

    uint32_t maxCrossTileID = 0;
    uint32_t maxBucketInstanceId = 0;
    CrossTileSymbolLayerIndex index(maxCrossTileID);

    Immutable<style::SymbolLayoutProperties::PossiblyEvaluated> layout =
        makeMutable<style::SymbolLayoutProperties::PossiblyEvaluated>();
    bool iconsNeedLinear = false;
    bool sortFeaturesByY = false;
    std::string bucketLeaderID = "test";

    OverscaledTileID mainID(6, 0, 6, 8, 8);
    std::vector<SymbolInstance> mainInstances;
    mainInstances.push_back(makeSymbolInstance(1000, 1000, u"Detroit"));
    mainInstances.push_back(makeSymbolInstance(2000, 2000, u"Toronto"));
    SymbolBucket mainBucket{layout,
                            {},
                            16.0f,
                            1.0f,
                            0,
                            iconsNeedLinear,
                            sortFeaturesByY,
                            bucketLeaderID,
                            std::move(mainInstances),
                            1.0f,
                            false,
                            {},
                            false /*iconsInText*/};
    mainBucket.bucketInstanceId = ++maxBucketInstanceId;
    index.addBucket(mainID, mainBucket);

    // Assigned new IDs
    ASSERT_EQ(mainBucket.symbolInstances.at(0).crossTileID, 1u);
    ASSERT_EQ(mainBucket.symbolInstances.at(1).crossTileID, 2u);


    OverscaledTileID childID(7, 0, 7, 16, 16);
    std::vector<SymbolInstance> childInstances;
    childInstances.push_back(makeSymbolInstance(2000, 2000, u"Detroit"));
    childInstances.push_back(makeSymbolInstance(2000, 2000, u"Windsor"));
    childInstances.push_back(makeSymbolInstance(3000, 3000, u"Toronto"));
    childInstances.push_back(makeSymbolInstance(4001, 4001, u"Toronto"));
    SymbolBucket childBucket{layout,
                             {},
                             16.0f,
                             1.0f,
                             0,
                             iconsNeedLinear,
                             sortFeaturesByY,
                             bucketLeaderID,
                             std::move(childInstances),
                             1.0f,
                             false,
                             {},
                             false /*iconsInText*/};
    childBucket.bucketInstanceId = ++maxBucketInstanceId;
    index.addBucket(childID, childBucket);

    // matched parent tile
    ASSERT_EQ(childBucket.symbolInstances.at(0).crossTileID, 1u);
    // does not match because of different key
    ASSERT_EQ(childBucket.symbolInstances.at(1).crossTileID, 3u);
    // does not match because of different location
    ASSERT_EQ(childBucket.symbolInstances.at(2).crossTileID, 4u);
    // matches with a slightly different location
    ASSERT_EQ(childBucket.symbolInstances.at(3).crossTileID, 2u);

    OverscaledTileID parentID(5, 0, 5, 4, 4);
    std::vector<SymbolInstance> parentInstances;
    parentInstances.push_back(makeSymbolInstance(500, 500, u"Detroit"));
    SymbolBucket parentBucket{layout,
                              {},
                              16.0f,
                              1.0f,
                              0,
                              iconsNeedLinear,
                              sortFeaturesByY,
                              bucketLeaderID,
                              std::move(parentInstances),
                              1.0f,
                              false,
                              {},
                              false /*iconsInText*/};
    parentBucket.bucketInstanceId = ++maxBucketInstanceId;
    index.addBucket(parentID, parentBucket);

    // matched child tile
    ASSERT_EQ(parentBucket.symbolInstances.at(0).crossTileID, 1u);

    std::unordered_set<uint32_t> currentIDs;
    currentIDs.insert(mainBucket.bucketInstanceId);
    index.removeStaleBuckets(currentIDs);

    // grandchild
    OverscaledTileID grandchildID(8, 0, 8, 32, 32);
    std::vector<SymbolInstance> grandchildInstances;
    grandchildInstances.push_back(makeSymbolInstance(4000, 4000, u"Detroit"));
    grandchildInstances.push_back(makeSymbolInstance(4000, 4000, u"Windsor"));
    SymbolBucket grandchildBucket{layout,
                                  {},
                                  16.0f,
                                  1.0f,
                                  0,
                                  iconsNeedLinear,
                                  sortFeaturesByY,
                                  bucketLeaderID,
                                  std::move(grandchildInstances),
                                  1.0f,
                                  false,
                                  {},
                                  false /*iconsInText*/};
    grandchildBucket.bucketInstanceId = ++maxBucketInstanceId;
    index.addBucket(grandchildID, grandchildBucket);

    // Matches the symbol in `mainBucket`
    ASSERT_EQ(grandchildBucket.symbolInstances.at(0).crossTileID, 1u);
    // Does not match the previous value for Windsor because that tile was removed
    ASSERT_EQ(grandchildBucket.symbolInstances.at(1).crossTileID, 5u);

}

TEST(CrossTileSymbolLayerIndex, resetIDs) {

    uint32_t maxCrossTileID = 0;
    uint32_t maxBucketInstanceId = 0;
    CrossTileSymbolLayerIndex index(maxCrossTileID);

    Immutable<style::SymbolLayoutProperties::PossiblyEvaluated> layout =
        makeMutable<style::SymbolLayoutProperties::PossiblyEvaluated>();
    bool iconsNeedLinear = false;
    bool sortFeaturesByY = false;
    std::string bucketLeaderID = "test";

    OverscaledTileID mainID(6, 0, 6, 8, 8);
    std::vector<SymbolInstance> mainInstances;
    mainInstances.push_back(makeSymbolInstance(1000, 1000, u"Detroit"));
    SymbolBucket mainBucket{layout,
                            {},
                            16.0f,
                            1.0f,
                            0,
                            iconsNeedLinear,
                            sortFeaturesByY,
                            bucketLeaderID,
                            std::move(mainInstances),
                            1.0f,
                            false,
                            {},
                            false /*iconsInText*/};
    mainBucket.bucketInstanceId = ++maxBucketInstanceId;

    OverscaledTileID childID(7, 0, 7, 16, 16);
    std::vector<SymbolInstance> childInstances;
    childInstances.push_back(makeSymbolInstance(2000, 2000, u"Detroit"));
    SymbolBucket childBucket{layout,
                             {},
                             16.0f,
                             1.0f,
                             0,
                             iconsNeedLinear,
                             sortFeaturesByY,
                             bucketLeaderID,
                             std::move(childInstances),
                             1.0f,
                             false,
                             {},
                             false /*iconsInText*/};
    childBucket.bucketInstanceId = ++maxBucketInstanceId;

    // assigns a new id
    index.addBucket(mainID, mainBucket);
    ASSERT_EQ(mainBucket.symbolInstances.at(0).crossTileID, 1u);

    // removes the tile
    std::unordered_set<uint32_t> currentIDs;
    index.removeStaleBuckets(currentIDs);

    // assigns a new id
    index.addBucket(childID, childBucket);
    ASSERT_EQ(childBucket.symbolInstances.at(0).crossTileID, 2u);

    // overwrites the old id to match the already-added tile
    index.addBucket(mainID, mainBucket);
    ASSERT_EQ(mainBucket.symbolInstances.at(0).crossTileID, 2u);
}

TEST(CrossTileSymbolLayerIndex, noDuplicatesWithinZoomLevel) {
    uint32_t maxCrossTileID = 0;
    uint32_t maxBucketInstanceId = 0;
    CrossTileSymbolLayerIndex index(maxCrossTileID);

    Immutable<style::SymbolLayoutProperties::PossiblyEvaluated> layout =
        makeMutable<style::SymbolLayoutProperties::PossiblyEvaluated>();
    bool iconsNeedLinear = false;
    bool sortFeaturesByY = false;
    std::string bucketLeaderID = "test";

    OverscaledTileID mainID(6, 0, 6, 8, 8);
    std::vector<SymbolInstance> mainInstances;
    mainInstances.push_back(makeSymbolInstance(1000, 1000, u"")); // A
    mainInstances.push_back(makeSymbolInstance(1000, 1000, u"")); // B
    SymbolBucket mainBucket{layout,
                            {},
                            16.0f,
                            1.0f,
                            0,
                            iconsNeedLinear,
                            sortFeaturesByY,
                            bucketLeaderID,
                            std::move(mainInstances),
                            1.0f,
                            false,
                            {},
                            false /*iconsInText*/};
    mainBucket.bucketInstanceId = ++maxBucketInstanceId;

    OverscaledTileID childID(7, 0, 7, 16, 16);
    std::vector<SymbolInstance> childInstances;
    childInstances.push_back(makeSymbolInstance(2000, 2000, u"")); // A'
    childInstances.push_back(makeSymbolInstance(2000, 2000, u"")); // B'
    childInstances.push_back(makeSymbolInstance(2000, 2000, u"")); // C'
    SymbolBucket childBucket{layout,
                             {},
                             16.0f,
                             1.0f,
                             0,
                             iconsNeedLinear,
                             sortFeaturesByY,
                             bucketLeaderID,
                             std::move(childInstances),
                             1.0f,
                             false,
                             {},
                             false /*iconsInText*/};
    childBucket.bucketInstanceId = ++maxBucketInstanceId;

    // assigns new ids
    index.addBucket(mainID, mainBucket);
    ASSERT_EQ(mainBucket.symbolInstances.at(0).crossTileID, 1u);
    ASSERT_EQ(mainBucket.symbolInstances.at(1).crossTileID, 2u);

    // copies parent ids without duplicate ids in this tile
    index.addBucket(childID, childBucket);
    ASSERT_EQ(childBucket.symbolInstances.at(0).crossTileID, 1u); // A' copies from A
    ASSERT_EQ(childBucket.symbolInstances.at(1).crossTileID, 2u); // B' copies from B
    ASSERT_EQ(childBucket.symbolInstances.at(2).crossTileID, 3u); // C' gets new ID
}

TEST(CrossTileSymbolLayerIndex, bucketReplacement) {
    uint32_t maxCrossTileID = 0;
    uint32_t maxBucketInstanceId = 0;
    CrossTileSymbolLayerIndex index(maxCrossTileID);

    Immutable<style::SymbolLayoutProperties::PossiblyEvaluated> layout =
        makeMutable<style::SymbolLayoutProperties::PossiblyEvaluated>();
    bool iconsNeedLinear = false;
    bool sortFeaturesByY = false;
    std::string bucketLeaderID = "test";

    OverscaledTileID tileID(6, 0, 6, 8, 8);
    std::vector<SymbolInstance> firstInstances;
    firstInstances.push_back(makeSymbolInstance(1000, 1000, u"")); // A
    firstInstances.push_back(makeSymbolInstance(1000, 1000, u"")); // B
    SymbolBucket firstBucket{layout,
                             {},
                             16.0f,
                             1.0f,
                             0,
                             iconsNeedLinear,
                             sortFeaturesByY,
                             bucketLeaderID,
                             std::move(firstInstances),
                             1.0f,
                             false,
                             {},
                             false /*iconsInText*/};
    firstBucket.bucketInstanceId = ++maxBucketInstanceId;

    std::vector<SymbolInstance> secondInstances;
    secondInstances.push_back(makeSymbolInstance(1000, 1000, u"")); // A'
    secondInstances.push_back(makeSymbolInstance(1000, 1000, u"")); // B'
    secondInstances.push_back(makeSymbolInstance(1000, 1000, u"")); // C'
    SymbolBucket secondBucket{layout,
                              {},
                              16.0f,
                              1.0f,
                              0,
                              iconsNeedLinear,
                              sortFeaturesByY,
                              bucketLeaderID,
                              std::move(secondInstances),
                              1.0f,
                              false,
                              {},
                              false /*iconsInText*/};
    secondBucket.bucketInstanceId = ++maxBucketInstanceId;

    // assigns new ids
    index.addBucket(tileID, firstBucket);
    ASSERT_EQ(firstBucket.symbolInstances.at(0).crossTileID, 1u);
    ASSERT_EQ(firstBucket.symbolInstances.at(1).crossTileID, 2u);

    // copies parent ids without duplicate ids in this tile
    index.addBucket(tileID, secondBucket);
    ASSERT_EQ(secondBucket.symbolInstances.at(0).crossTileID, 1u); // A' copies from A
    ASSERT_EQ(secondBucket.symbolInstances.at(1).crossTileID, 2u); // B' copies from B
    ASSERT_EQ(secondBucket.symbolInstances.at(2).crossTileID, 3u); // C' gets new ID
}

