#include <mbgl/util/grid_index.hpp>
#include <mbgl/util/grid_index.cpp>

#include <mbgl/test/util.hpp>

using namespace mbgl;

TEST(GridIndex, IndexesFeatures) {
    GridIndex<int16_t> grid(100, 100, 10);
    grid.insert(0, {{4, 10}, {6, 30}});
    grid.insert(1, {{4, 10}, {30, 12}});
    grid.insert(2, {{-10, 30}, {5, 35}});
    
    EXPECT_EQ(grid.query({{4, 10}, {5, 11}}), (std::vector<int16_t>{0, 1}));
    EXPECT_EQ(grid.query({{24, 10}, {25, 11}}), (std::vector<int16_t>{1}));
    EXPECT_EQ(grid.query({{40, 40}, {100, 100}}), (std::vector<int16_t>{}));
    EXPECT_EQ(grid.query({{-6, 0}, {3, 100}}), (std::vector<int16_t>{2}));
    EXPECT_EQ(grid.query({{-1000, -1000}, {1000, 1000}}), (std::vector<int16_t>{0, 1, 2}));
}

TEST(GridIndex, DuplicateKeys) {
    GridIndex<int16_t> grid(100, 100, 10);
    #define KEY 123
    grid.insert(KEY, {{3, 4}, {4, 4}});
    grid.insert(KEY, {{13, 13}, {14, 14}});
    grid.insert(KEY, {{23, 23}, {24, 24}});
    
    EXPECT_EQ(grid.query({{0, 0}, {30, 30}}), (std::vector<int16_t>{KEY, KEY, KEY}));
}

TEST(GridIndex, CircleCircle) {
    GridIndex<int16_t> grid(100, 100, 10);
    grid.insert(0, {{50, 50}, 10});
    grid.insert(1, {{60, 60}, 15});
    grid.insert(2, {{-10, 110}, 20});
    
    EXPECT_TRUE(grid.hitTest({{55, 55}, 2}));
    EXPECT_FALSE(grid.hitTest({{10, 10}, 10}));
    EXPECT_TRUE(grid.hitTest({{0, 100}, 10}));
    EXPECT_TRUE(grid.hitTest({{80, 60}, 10}));
}

TEST(GridIndex, CircleBox) {
    GridIndex<int16_t> grid(100, 100, 10);
    grid.insert(0, {{50, 50}, 10});
    grid.insert(1, {{60, 60}, 15});
    grid.insert(2, {{-10, 110}, 20});
    
    EXPECT_EQ(grid.query({{45, 45}, {55, 55}}), (std::vector<int16_t>{0, 1}));
    EXPECT_EQ(grid.query({{0, 0}, {30, 30}}), (std::vector<int16_t>{}));
    EXPECT_EQ(grid.query({{0, 80}, {20, 100}}), (std::vector<int16_t>{2}));
}

