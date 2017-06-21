#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_layer_observer.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/style/style_impl.hpp>
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
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/io.hpp>

#include <memory>

using namespace mbgl;
using namespace mbgl::style;

namespace {

const auto color = Color { 1, 0, 0, 1 };
const auto opacity = 1.0f;
const auto radius = 1.0f;
const auto blur = 1.0f;
const auto pattern = std::string { "foo" };
const auto antialias = false;
const auto translate = std::array<float, 2> {{ 0, 0 }};
const auto translateAnchor = TranslateAnchorType::Map;
const auto lineCap = LineCapType::Round;
const auto lineJoin = LineJoinType::Miter;
const auto miterLimit = 1.0f;
const auto roundLimit = 1.0f;
const auto width = 1.0f;
const auto gapWidth = 1.0f;
const auto offset = 1.0f;
const auto dashArray = std::vector<float> {};
const auto hueRotate = 1.0f;
const auto brightness = 1.0f;
const auto saturation = 1.0f;
const auto contrast = 1.0f;
const auto duration = 1.0f;

} // namespace

TEST(Layer, BackgroundProperties) {
    auto layer = std::make_unique<BackgroundLayer>("background");
    EXPECT_TRUE(layer->is<BackgroundLayer>());

    // Paint properties

    layer->setBackgroundColor(color);
    EXPECT_EQ(layer->getBackgroundColor(), color);

    layer->setBackgroundOpacity(opacity);
    EXPECT_EQ(layer->getBackgroundOpacity(), opacity);

    layer->setBackgroundPattern(pattern);
    EXPECT_EQ(layer->getBackgroundPattern(), pattern);
}

TEST(Layer, CircleProperties) {
    auto layer = std::make_unique<CircleLayer>("circle", "source");
    EXPECT_TRUE(layer->is<CircleLayer>());

    // Paint properties

    layer->setCircleColor(color);
    EXPECT_EQ(layer->getCircleColor(), color);

    layer->setCircleOpacity(opacity);
    EXPECT_EQ(layer->getCircleOpacity(), opacity);

    layer->setCircleRadius(radius);
    EXPECT_EQ(layer->getCircleRadius(), radius);

    layer->setCircleBlur(blur);
    EXPECT_EQ(layer->getCircleBlur(), blur);

    layer->setCircleTranslate(translate);
    EXPECT_EQ(layer->getCircleTranslate(), translate);

    layer->setCircleTranslateAnchor(translateAnchor);
    EXPECT_EQ(layer->getCircleTranslateAnchor(), translateAnchor);
}

TEST(Layer, FillProperties) {
    auto layer = std::make_unique<FillLayer>("fill", "source");
    EXPECT_TRUE(layer->is<FillLayer>());

    // Paint properties

    layer->setFillColor(color);
    EXPECT_EQ(layer->getFillColor(), color);

    layer->setFillOutlineColor(color);
    EXPECT_EQ(layer->getFillOutlineColor(), color);

    layer->setFillOpacity(opacity);
    EXPECT_EQ(layer->getFillOpacity(), opacity);

    layer->setFillPattern(pattern);
    EXPECT_EQ(layer->getFillPattern(), pattern);

    layer->setFillAntialias(antialias);
    EXPECT_EQ(layer->getFillAntialias(), antialias);

    layer->setFillTranslate(translate);
    EXPECT_EQ(layer->getFillTranslate(), translate);

    layer->setFillTranslateAnchor(translateAnchor);
    EXPECT_EQ(layer->getFillTranslateAnchor(), translateAnchor);
}

TEST(Layer, LineProperties) {
    auto layer = std::make_unique<LineLayer>("line", "source");
    EXPECT_TRUE(layer->is<LineLayer>());

    // Layout properties

    layer->setLineCap(lineCap);
    EXPECT_EQ(layer->getLineCap(), lineCap);

    layer->setLineJoin(lineJoin);
    EXPECT_EQ(layer->getLineJoin(), lineJoin);

    layer->setLineMiterLimit(miterLimit);
    EXPECT_EQ(layer->getLineMiterLimit(), miterLimit);

    layer->setLineRoundLimit(roundLimit);
    EXPECT_EQ(layer->getLineRoundLimit(), roundLimit);

    // Paint properties

    layer->setLineColor(color);
    EXPECT_EQ(layer->getLineColor(), color);

    layer->setLineOpacity(opacity);
    EXPECT_EQ(layer->getLineOpacity(), opacity);

    layer->setLineTranslate(translate);
    EXPECT_EQ(layer->getLineTranslate(), translate);

    layer->setLineTranslateAnchor(translateAnchor);
    EXPECT_EQ(layer->getLineTranslateAnchor(), translateAnchor);

    layer->setLineWidth(width);
    EXPECT_EQ(layer->getLineWidth(), width);

    layer->setLineGapWidth(gapWidth);
    EXPECT_EQ(layer->getLineGapWidth(), gapWidth);

    layer->setLineOffset(offset);
    EXPECT_EQ(layer->getLineOffset(), offset);

    layer->setLineBlur(blur);
    EXPECT_EQ(layer->getLineBlur(), blur);

    layer->setLineDasharray(dashArray);
    EXPECT_EQ(layer->getLineDasharray(), dashArray);

    layer->setLinePattern(pattern);
    EXPECT_EQ(layer->getLinePattern(), pattern);
}

TEST(Layer, RasterProperties) {
    auto layer = std::make_unique<RasterLayer>("raster", "source");
    EXPECT_TRUE(layer->is<RasterLayer>());

    // Paint properties

    layer->setRasterOpacity(opacity);
    EXPECT_EQ(layer->getRasterOpacity(), opacity);

    layer->setRasterHueRotate(hueRotate);
    EXPECT_EQ(layer->getRasterHueRotate(), hueRotate);

    layer->setRasterBrightnessMin(brightness);
    EXPECT_EQ(layer->getRasterBrightnessMin(), brightness);

    layer->setRasterBrightnessMax(brightness);
    EXPECT_EQ(layer->getRasterBrightnessMax(), brightness);

    layer->setRasterSaturation(saturation);
    EXPECT_EQ(layer->getRasterSaturation(), saturation);

    layer->setRasterContrast(contrast);
    EXPECT_EQ(layer->getRasterContrast(), contrast);

    layer->setRasterFadeDuration(duration);
    EXPECT_EQ(layer->getRasterFadeDuration(), duration);
}

TEST(Layer, Observer) {
    auto layer = std::make_unique<LineLayer>("line", "source");
    StubLayerObserver observer;
    layer->setObserver(&observer);

    // Notifies observer on filter change.
    bool filterChanged = false;
    observer.layerChanged = [&] (Layer& layer_) {
        EXPECT_EQ(layer.get(), &layer_);
        filterChanged = true;
    };
    layer->setFilter(NullFilter());
    EXPECT_TRUE(filterChanged);

    // Notifies observer on visibility change.
    bool visibilityChanged = false;
    observer.layerChanged = [&] (Layer& layer_) {
        EXPECT_EQ(layer.get(), &layer_);
        visibilityChanged = true;
    };
    layer->setVisibility(VisibilityType::None);
    EXPECT_TRUE(visibilityChanged);

    // Notifies observer on paint property change.
    bool paintPropertyChanged = false;
    observer.layerChanged = [&] (Layer& layer_) {
        EXPECT_EQ(layer.get(), &layer_);
        paintPropertyChanged = true;
    };
    layer->setLineColor(color);
    EXPECT_TRUE(paintPropertyChanged);

    // Notifies observer on layout property change.
    bool layoutPropertyChanged = false;
    observer.layerChanged = [&] (Layer& layer_) {
        EXPECT_EQ(layer.get(), &layer_);
        layoutPropertyChanged = true;
    };
    layer->setLineCap(lineCap);
    EXPECT_TRUE(layoutPropertyChanged);

    // Does not notify observer on no-op visibility change.
    visibilityChanged = false;
    observer.layerChanged = [&] (Layer& layer_) {
        EXPECT_EQ(layer.get(), &layer_);
        visibilityChanged = true;
    };
    layer->setVisibility(VisibilityType::None);
    EXPECT_FALSE(visibilityChanged);

    // Does not notify observer on no-op paint property change.
    paintPropertyChanged = false;
    observer.layerChanged = [&] (Layer& layer_) {
        EXPECT_EQ(layer.get(), &layer_);
        paintPropertyChanged = true;
    };
    layer->setLineColor(color);
    EXPECT_FALSE(paintPropertyChanged);

    // Does not notify observer on no-op layout property change.
    layoutPropertyChanged = false;
    observer.layerChanged = [&] (Layer& layer_) {
        EXPECT_EQ(layer.get(), &layer_);
        layoutPropertyChanged = true;
    };
    layer->setLineCap(lineCap);
    EXPECT_FALSE(layoutPropertyChanged);
}

TEST(Layer, DuplicateLayer) {
    util::RunLoop loop;

    // Setup style
    ThreadPool threadPool{ 1 };
    StubFileSource fileSource;
    Style::Impl style { threadPool, fileSource, 1.0 };
    style.loadJSON(util::read_file("test/fixtures/resources/style-unused-sources.json"));

    // Add initial layer
    style.addLayer(std::make_unique<LineLayer>("line", "unusedsource"));

    // Try to add duplicate
    try {
        style.addLayer(std::make_unique<LineLayer>("line", "unusedsource"));
        FAIL() << "Should not have been allowed to add a duplicate layer id";
    } catch (const std::runtime_error e) {
        // Expected
        ASSERT_STREQ("Layer line already exists", e.what());
    }
}

