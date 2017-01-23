#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_layer_observer.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/io.hpp>

#include <memory>

using namespace mbgl;
using namespace mbgl::style;

namespace {

template <class T, class... Params> void testClone(Params... params) {
    auto layer = std::make_unique<T>(std::forward<Params>(params)...);
    auto clone = layer->baseImpl->clone();
    EXPECT_NE(layer.get(), clone.get());
    EXPECT_TRUE(reinterpret_cast<typename T::Impl*>(clone->baseImpl.get()));
    layer->impl->id = "test";
    EXPECT_EQ("test", layer->baseImpl->clone()->getID());
}

const auto color = PropertyValue<Color> {{ 1, 0, 0, 1 }};
const auto opacity = PropertyValue<float> { 1.0f };
const auto radius = PropertyValue<float> { 1.0f };
const auto blur = PropertyValue<float> { 1.0f };
const auto pattern = PropertyValue<std::string> { "foo" };
const auto antialias = PropertyValue<bool> { false };
const auto translate = PropertyValue<std::array<float, 2>> {{{ 0, 0 }}};
const auto translateAnchor = PropertyValue<TranslateAnchorType> { TranslateAnchorType::Map };
const auto lineCap = PropertyValue<LineCapType> { LineCapType::Round };
const auto lineJoin = PropertyValue<LineJoinType> { LineJoinType::Miter };
const auto miterLimit = PropertyValue<float> { 1.0f };
const auto roundLimit = PropertyValue<float> { 1.0f };
const auto width = PropertyValue<float> { 1.0f };
const auto gapWidth = PropertyValue<float> { 1.0f };
const auto offset = PropertyValue<float> { 1.0f };
const auto dashArray = PropertyValue<std::vector<float>> {{}};
const auto hueRotate = PropertyValue<float> { 1.0f };
const auto brightness = PropertyValue<float> { 1.0f };
const auto saturation = PropertyValue<float> { 1.0f };
const auto contrast = PropertyValue<float> { 1.0f };
const auto duration = PropertyValue<float> { 1.0f };

} // namespace

TEST(Layer, Clone) {
    testClone<BackgroundLayer>("background");
    testClone<CircleLayer>("circle", "source");
    testClone<CustomLayer>("custom", [](void*){}, [](void*, const CustomLayerRenderParameters&){}, [](void*){}, nullptr),
    testClone<FillLayer>("fill", "source");
    testClone<LineLayer>("line", "source");
    testClone<RasterLayer>("raster", "source");
    testClone<SymbolLayer>("symbol", "source");
}

TEST(Layer, BackgroundProperties) {
    auto layer = std::make_unique<BackgroundLayer>("background");
    EXPECT_TRUE(layer->is<BackgroundLayer>());

    // Paint properties

    layer->setBackgroundColor(color);
    EXPECT_EQ(layer->getBackgroundColor().asConstant(), color.asConstant());

    layer->setBackgroundOpacity(opacity);
    EXPECT_EQ(layer->getBackgroundOpacity().asConstant(), opacity.asConstant());

    layer->setBackgroundPattern(pattern);
    EXPECT_EQ(layer->getBackgroundPattern().asConstant(), pattern.asConstant());
}

TEST(Layer, CircleProperties) {
    auto layer = std::make_unique<CircleLayer>("circle", "source");
    EXPECT_TRUE(layer->is<CircleLayer>());

    // Paint properties

    layer->setCircleColor(color);
    EXPECT_EQ(layer->getCircleColor().asConstant(), color.asConstant());

    layer->setCircleOpacity(opacity);
    EXPECT_EQ(layer->getCircleOpacity().asConstant(), opacity.asConstant());

    layer->setCircleRadius(radius);
    EXPECT_EQ(layer->getCircleRadius().asConstant(), radius.asConstant());

    layer->setCircleBlur(blur);
    EXPECT_EQ(layer->getCircleBlur().asConstant(), blur.asConstant());

    layer->setCircleTranslate(translate);
    EXPECT_EQ(layer->getCircleTranslate().asConstant(), translate.asConstant());

    layer->setCircleTranslateAnchor(translateAnchor);
    EXPECT_EQ(layer->getCircleTranslateAnchor().asConstant(), translateAnchor.asConstant());
}

TEST(Layer, FillProperties) {
    auto layer = std::make_unique<FillLayer>("fill", "source");
    EXPECT_TRUE(layer->is<FillLayer>());

    // Paint properties

    layer->setFillColor(color);
    EXPECT_EQ(layer->getFillColor().asConstant(), color.asConstant());

    layer->setFillOutlineColor(color);
    EXPECT_EQ(layer->getFillOutlineColor().asConstant(), color.asConstant());

    layer->setFillOpacity(opacity);
    EXPECT_EQ(layer->getFillOpacity().asConstant(), opacity.asConstant());

    layer->setFillPattern(pattern);
    EXPECT_EQ(layer->getFillPattern().asConstant(), pattern.asConstant());

    layer->setFillAntialias(antialias);
    EXPECT_EQ(layer->getFillAntialias().asConstant(), antialias.asConstant());

    layer->setFillTranslate(translate);
    EXPECT_EQ(layer->getFillTranslate().asConstant(), translate.asConstant());

    layer->setFillTranslateAnchor(translateAnchor);
    EXPECT_EQ(layer->getFillTranslateAnchor().asConstant(), translateAnchor.asConstant());
}

TEST(Layer, LineProperties) {
    auto layer = std::make_unique<LineLayer>("line", "source");
    EXPECT_TRUE(layer->is<LineLayer>());

    // Layout properties

    layer->setLineCap(lineCap);
    EXPECT_EQ(layer->getLineCap().asConstant(), lineCap.asConstant());

    layer->setLineJoin(lineJoin);
    EXPECT_EQ(layer->getLineJoin().asConstant(), lineJoin.asConstant());

    layer->setLineMiterLimit(miterLimit);
    EXPECT_EQ(layer->getLineMiterLimit().asConstant(), miterLimit.asConstant());

    layer->setLineRoundLimit(roundLimit);
    EXPECT_EQ(layer->getLineRoundLimit().asConstant(), roundLimit.asConstant());

    // Paint properties

    layer->setLineColor(color);
    EXPECT_EQ(layer->getLineColor().asConstant(), color.asConstant());

    layer->setLineOpacity(opacity);
    EXPECT_EQ(layer->getLineOpacity().asConstant(), opacity.asConstant());

    layer->setLineTranslate(translate);
    EXPECT_EQ(layer->getLineTranslate().asConstant(), translate.asConstant());

    layer->setLineTranslateAnchor(translateAnchor);
    EXPECT_EQ(layer->getLineTranslateAnchor().asConstant(), translateAnchor.asConstant());

    layer->setLineWidth(width);
    EXPECT_EQ(layer->getLineWidth().asConstant(), width.asConstant());

    layer->setLineGapWidth(gapWidth);
    EXPECT_EQ(layer->getLineGapWidth().asConstant(), gapWidth.asConstant());

    layer->setLineOffset(offset);
    EXPECT_EQ(layer->getLineOffset().asConstant(), offset.asConstant());

    layer->setLineBlur(blur);
    EXPECT_EQ(layer->getLineBlur().asConstant(), blur.asConstant());

    layer->setLineDasharray(dashArray);
    EXPECT_EQ(layer->getLineDasharray().asConstant(), dashArray.asConstant());

    layer->setLinePattern(pattern);
    EXPECT_EQ(layer->getLinePattern().asConstant(), pattern.asConstant());
}

TEST(Layer, RasterProperties) {
    auto layer = std::make_unique<RasterLayer>("raster", "source");
    EXPECT_TRUE(layer->is<RasterLayer>());

    // Paint properties

    layer->setRasterOpacity(opacity);
    EXPECT_EQ(layer->getRasterOpacity().asConstant(), opacity.asConstant());

    layer->setRasterHueRotate(hueRotate);
    EXPECT_EQ(layer->getRasterHueRotate().asConstant(), hueRotate.asConstant());

    layer->setRasterBrightnessMin(brightness);
    EXPECT_EQ(layer->getRasterBrightnessMin().asConstant(), brightness.asConstant());

    layer->setRasterBrightnessMax(brightness);
    EXPECT_EQ(layer->getRasterBrightnessMax().asConstant(), brightness.asConstant());

    layer->setRasterSaturation(saturation);
    EXPECT_EQ(layer->getRasterSaturation().asConstant(), saturation.asConstant());

    layer->setRasterContrast(contrast);
    EXPECT_EQ(layer->getRasterContrast().asConstant(), contrast.asConstant());

    layer->setRasterFadeDuration(duration);
    EXPECT_EQ(layer->getRasterFadeDuration().asConstant(), duration.asConstant());
}

TEST(Layer, Observer) {
    auto layer = std::make_unique<LineLayer>("line", "source");
    StubLayerObserver observer;
    layer->baseImpl->setObserver(&observer);

    // Notifies observer on filter change.
    bool filterChanged = false;
    observer.layerFilterChanged = [&] (Layer& layer_) {
        EXPECT_EQ(layer.get(), &layer_);
        filterChanged = true;
    };
    layer->setFilter(NullFilter());
    EXPECT_TRUE(filterChanged);

    // Notifies observer on visibility change.
    bool visibilityChanged = false;
    observer.layerVisibilityChanged = [&] (Layer& layer_) {
        EXPECT_EQ(layer.get(), &layer_);
        visibilityChanged = true;
    };
    layer->setVisibility(VisibilityType::None);
    EXPECT_TRUE(visibilityChanged);

    // Notifies observer on paint property change.
    bool paintPropertyChanged = false;
    observer.layerPaintPropertyChanged = [&] (Layer& layer_) {
        EXPECT_EQ(layer.get(), &layer_);
        paintPropertyChanged = true;
    };
    layer->setLineColor(color);
    EXPECT_TRUE(paintPropertyChanged);

    // Notifies observer on layout property change.
    bool layoutPropertyChanged = false;
    observer.layerLayoutPropertyChanged = [&] (Layer& layer_, const char *) {
        EXPECT_EQ(layer.get(), &layer_);
        layoutPropertyChanged = true;
    };
    layer->setLineCap(lineCap);
    EXPECT_TRUE(layoutPropertyChanged);

    // Does not notify observer on no-op visibility change.
    visibilityChanged = false;
    layer->setVisibility(VisibilityType::None);
    EXPECT_FALSE(visibilityChanged);

    // Does not notify observer on no-op paint property change.
    paintPropertyChanged = false;
    layer->setLineColor(color);
    EXPECT_FALSE(paintPropertyChanged);

    // Does not notify observer on no-op layout property change.
    layoutPropertyChanged = false;
    layer->setLineCap(lineCap);
    EXPECT_FALSE(layoutPropertyChanged);
}

TEST(Layer, DuplicateLayer) {
    util::RunLoop loop;

    //Setup style
    StubFileSource fileSource;
    Style style { fileSource, 1.0 };
    style.setJSON(util::read_file("test/fixtures/resources/style-unused-sources.json"));

    //Add initial layer
    style.addLayer(std::make_unique<LineLayer>("line", "unusedsource"));

    //Try to add duplicate
    try {
        style.addLayer(std::make_unique<LineLayer>("line", "unusedsource"));
        FAIL() << "Should not have been allowed to add a duplicate layer id";
    } catch (const std::runtime_error e) {
        //Expected
        ASSERT_STREQ("Layer line already exists", e.what());
    }
}

