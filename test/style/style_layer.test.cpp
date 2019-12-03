#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/expression/format_expression.hpp>
#include <mbgl/style/expression/image.hpp>
#include <mbgl/style/expression/match.hpp>
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
#include <mbgl/style/style_impl.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/stub_layer_observer.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

#include <memory>

using namespace mbgl;
using namespace mbgl::style;
using namespace expression;
using namespace expression::dsl;
using namespace std::literals::string_literals;

namespace {

const auto color = Color { 1, 0, 0, 1 };
const auto opacity = 1.0f;
const auto radius = 1.0f;
const auto blur = 1.0f;
const auto pattern = PropertyValue<expression::Image>{"foo"};
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

class MockLayoutProperties : public Properties<TextField> {};
class MockPaintProperties : public Properties<TextColor> {};
using MockOverrides = FormatSectionOverrides<MockPaintProperties::OverridableProperties>;

} // namespace

TEST(Layer, BackgroundProperties) {
    auto layer = std::make_unique<BackgroundLayer>("background");
    ASSERT_STREQ("background", layer->getTypeInfo()->type);

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
    ASSERT_STREQ("circle", layer->getTypeInfo()->type);

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
    ASSERT_STREQ("fill", layer->getTypeInfo()->type);

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
    ASSERT_STREQ("line", layer->getTypeInfo()->type);

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
    ASSERT_STREQ("raster", layer->getTypeInfo()->type);

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
    layer->setFilter(Filter());
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
    StubFileSource fileSource;
    Style::Impl style { fileSource, 1.0 };
    style.loadJSON(util::read_file("test/fixtures/resources/style-unused-sources.json"));

    // Add initial layer
    style.addLayer(std::make_unique<LineLayer>("line", "unusedsource"));

    // Try to add duplicate
    try {
        style.addLayer(std::make_unique<LineLayer>("line", "unusedsource"));
        FAIL() << "Should not have been allowed to add a duplicate layer id";
    } catch (const std::runtime_error& e) {
        // Expected
        ASSERT_STREQ("Layer line already exists", e.what());
    }
}

TEST(Layer, IncompatibleLayer) {
    util::RunLoop loop;

    // Setup style
    StubFileSource fileSource;
    Style::Impl style{fileSource, 1.0};
    style.loadJSON(util::read_file("test/fixtures/resources/style-unused-sources.json"));

    // Try to add duplicate
    try {
        style.addLayer(std::make_unique<RasterLayer>("raster", "unusedsource"));
        FAIL() << "Should not have been allowed to add an incompatible layer to the source";
    } catch (const std::runtime_error& e) {
        // Expected
        ASSERT_STREQ("Layer 'raster' is not compatible with source 'unusedsource'", e.what());
    }
}

namespace {

template<template<typename> class PropertyValueType, typename LayoutType>
void testHasOverrides(LayoutType& layout) {
    // Undefined
    layout.template get<TextField>() = PropertyValueType<Formatted>();
    EXPECT_FALSE(MockOverrides::hasOverrides(layout.template get<TextField>()));

    // Constant, no overrides.
    layout.template get<TextField>() = PropertyValueType<Formatted>(Formatted(""));
    EXPECT_FALSE(MockOverrides::hasOverrides(layout.template get<TextField>()));

    // Constant, overridden text-color.
    auto formatted = Formatted("");
    formatted.sections.emplace_back("section text"s, nullopt, nullopt, Color::green());
    layout.template get<TextField>() = PropertyValueType<Formatted>(std::move(formatted));
    EXPECT_TRUE(MockOverrides::hasOverrides(layout.template get<TextField>()));

    // Expression, no overrides.
    auto formatExpr = std::make_unique<FormatExpression>(std::vector<FormatExpressionSection>{});
    PropertyExpression<Formatted> propExpr(std::move(formatExpr));
    layout.template get<TextField>() = PropertyValueType<Formatted>(std::move(propExpr));
    EXPECT_FALSE(MockOverrides::hasOverrides(layout.template get<TextField>()));

    // Expression, overridden text-color.
    FormatExpressionSection section(literal(""));
    section.setTextSectionOptions(nullopt, nullopt, toColor(literal("red")));
    auto formatExprOverride = std::make_unique<FormatExpression>(std::vector<FormatExpressionSection>{section});
    PropertyExpression<Formatted> propExprOverride(std::move(formatExprOverride));
    layout.template get<TextField>() = PropertyValueType<Formatted>(std::move(propExprOverride));
    EXPECT_TRUE(MockOverrides::hasOverrides(layout.template get<TextField>()));

    // Nested expressions, overridden text-color.
    auto formattedExpr1 = format("first paragraph");
    FormatExpressionSection secondParagraph(literal("second paragraph"));
    secondParagraph.setTextSectionOptions(nullopt, nullopt, toColor(literal("blue")));
    std::vector<FormatExpressionSection> sections{{std::move(secondParagraph)}};
    auto formattedExpr2 = std::make_unique<FormatExpression>(std::move(sections));
    std::unordered_map<std::string, std::shared_ptr<Expression>> branches{ { "1st", std::move(formattedExpr1) },
                                                                           { "2nd", std::move(formattedExpr2) } };
    auto match = std::make_unique<Match<std::string>>(type::Formatted, literal("input"), std::move(branches), format("otherwise"));
    PropertyExpression<Formatted> nestedPropExpr(std::move(match));
    layout.template get<TextField>() = PropertyValueType<Formatted>(std::move(nestedPropExpr));
    EXPECT_TRUE(MockOverrides::hasOverrides(layout.template get<TextField>()));
}

} // namespace

TEST(Layer, SymbolLayerOverrides) {

    // Unevaluated / transitionable.
    {
        MockLayoutProperties::Unevaluated layout;
        testHasOverrides<PropertyValue>(layout);

        MockPaintProperties::Transitionable current;
        MockPaintProperties::Transitionable updated;
        current.get<TextColor>() = Transitionable<PropertyValue<Color>>{{Color::green()}, {}};
        updated.get<TextColor>() = Transitionable<PropertyValue<Color>>{{Color::green()}, {}};
        EXPECT_FALSE(MockOverrides::hasPaintPropertyDifference(current, updated));

        current.get<TextColor>() = Transitionable<PropertyValue<Color>>{{Color::red()}, {}};
        EXPECT_TRUE(MockOverrides::hasPaintPropertyDifference(current, updated));
    }

    // Possibly evaluated.
    {
        MockLayoutProperties::PossiblyEvaluated layout;
        MockPaintProperties::PossiblyEvaluated paint;
        testHasOverrides<PossiblyEvaluatedPropertyValue>(layout);

        // Constant, overridden text-color.
        auto formatted = Formatted("");
        formatted.sections.emplace_back("section text"s, nullopt, nullopt, Color::green());
        layout.get<TextField>() = PossiblyEvaluatedPropertyValue<Formatted>(std::move(formatted));
        paint.get<TextColor>() = PossiblyEvaluatedPropertyValue<Color>{Color::red()};
        EXPECT_TRUE(paint.get<TextColor>().isConstant());
        MockOverrides::setOverrides(layout, paint);
        EXPECT_FALSE(paint.get<TextColor>().isConstant());

        MockPaintProperties::PossiblyEvaluated updated;
        updated.get<TextColor>() = PossiblyEvaluatedPropertyValue<Color>{Color::red()};
        MockOverrides::updateOverrides(paint, updated);
        EXPECT_FALSE(updated.get<TextColor>().isConstant());
    }
}
