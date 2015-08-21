#include <iostream>
#include "../fixtures/util.hpp"

#include <algorithm>

#include <mbgl/style/types.hpp>

using namespace mbgl;

TEST(Enums, StyleLayerType) {
    ASSERT_EQ(StyleLayerType::Unknown, StyleLayerTypeClass("unknown"));
    ASSERT_EQ(StyleLayerType::Fill, StyleLayerTypeClass("fill"));
    ASSERT_EQ(StyleLayerType::Line, StyleLayerTypeClass("line"));
    ASSERT_EQ(StyleLayerType::Symbol, StyleLayerTypeClass("symbol"));
    ASSERT_EQ(StyleLayerType::Raster, StyleLayerTypeClass("raster"));
    ASSERT_EQ(StyleLayerType::Background, StyleLayerTypeClass("background"));

    ASSERT_EQ(StyleLayerType::Unknown, StyleLayerTypeClass(StyleLayerType::Unknown));
    ASSERT_EQ(StyleLayerType::Fill, StyleLayerTypeClass(StyleLayerType::Fill));
    ASSERT_EQ(StyleLayerType::Line, StyleLayerTypeClass(StyleLayerType::Line));
    ASSERT_EQ(StyleLayerType::Symbol, StyleLayerTypeClass(StyleLayerType::Symbol));
    ASSERT_EQ(StyleLayerType::Raster, StyleLayerTypeClass(StyleLayerType::Raster));
    ASSERT_EQ(StyleLayerType::Background, StyleLayerTypeClass(StyleLayerType::Background));

    ASSERT_EQ(StyleLayerTypeClass(StyleLayerType::Unknown), StyleLayerTypeClass(StyleLayerType::Unknown));
    ASSERT_EQ(StyleLayerTypeClass(StyleLayerType::Fill), StyleLayerTypeClass(StyleLayerType::Fill));
    ASSERT_EQ(StyleLayerTypeClass(StyleLayerType::Line), StyleLayerTypeClass(StyleLayerType::Line));
    ASSERT_EQ(StyleLayerTypeClass(StyleLayerType::Symbol), StyleLayerTypeClass(StyleLayerType::Symbol));
    ASSERT_EQ(StyleLayerTypeClass(StyleLayerType::Raster), StyleLayerTypeClass(StyleLayerType::Raster));
    ASSERT_EQ(StyleLayerTypeClass(StyleLayerType::Background), StyleLayerTypeClass(StyleLayerType::Background));

    ASSERT_FALSE(StyleLayerTypeClass("").valid());
    ASSERT_FALSE(StyleLayerTypeClass("Backgrund").valid());
}

