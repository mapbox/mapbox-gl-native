#include <iostream>
#include "gtest/gtest.h"

#include <algorithm>

#include <mbgl/style/types.hpp>

using namespace mbgl;

TEST(Enums, StyleLayerType) {
    std::cerr << StyleLayerType::Raster;

    ASSERT_EQ(StyleLayerType::Unknown, StyleLayerTypeClass("unknown"));
    ASSERT_EQ(StyleLayerType::Fill, StyleLayerTypeClass("fill"));
    ASSERT_EQ(StyleLayerType::Line, StyleLayerTypeClass("line"));
    ASSERT_EQ(StyleLayerType::Icon, StyleLayerTypeClass("icon"));
    ASSERT_EQ(StyleLayerType::Text, StyleLayerTypeClass("text"));
    ASSERT_EQ(StyleLayerType::Raster, StyleLayerTypeClass("raster"));
    ASSERT_EQ(StyleLayerType::Composite, StyleLayerTypeClass("composite"));
    ASSERT_EQ(StyleLayerType::Background, StyleLayerTypeClass("background"));

    ASSERT_EQ(StyleLayerType::Unknown, StyleLayerTypeClass(StyleLayerType::Unknown));
    ASSERT_EQ(StyleLayerType::Fill, StyleLayerTypeClass(StyleLayerType::Fill));
    ASSERT_EQ(StyleLayerType::Line, StyleLayerTypeClass(StyleLayerType::Line));
    ASSERT_EQ(StyleLayerType::Icon, StyleLayerTypeClass(StyleLayerType::Icon));
    ASSERT_EQ(StyleLayerType::Text, StyleLayerTypeClass(StyleLayerType::Text));
    ASSERT_EQ(StyleLayerType::Raster, StyleLayerTypeClass(StyleLayerType::Raster));
    ASSERT_EQ(StyleLayerType::Composite, StyleLayerTypeClass(StyleLayerType::Composite));
    ASSERT_EQ(StyleLayerType::Background, StyleLayerTypeClass(StyleLayerType::Background));

    ASSERT_EQ(StyleLayerTypeClass(StyleLayerType::Unknown), StyleLayerTypeClass(StyleLayerType::Unknown));
    ASSERT_EQ(StyleLayerTypeClass(StyleLayerType::Fill), StyleLayerTypeClass(StyleLayerType::Fill));
    ASSERT_EQ(StyleLayerTypeClass(StyleLayerType::Line), StyleLayerTypeClass(StyleLayerType::Line));
    ASSERT_EQ(StyleLayerTypeClass(StyleLayerType::Icon), StyleLayerTypeClass(StyleLayerType::Icon));
    ASSERT_EQ(StyleLayerTypeClass(StyleLayerType::Text), StyleLayerTypeClass(StyleLayerType::Text));
    ASSERT_EQ(StyleLayerTypeClass(StyleLayerType::Raster), StyleLayerTypeClass(StyleLayerType::Raster));
    ASSERT_EQ(StyleLayerTypeClass(StyleLayerType::Composite), StyleLayerTypeClass(StyleLayerType::Composite));
    ASSERT_EQ(StyleLayerTypeClass(StyleLayerType::Background), StyleLayerTypeClass(StyleLayerType::Background));

    ASSERT_EQ(false, StyleLayerTypeClass("").valid());
    ASSERT_EQ(false, StyleLayerTypeClass("Backgrund").valid());
}

