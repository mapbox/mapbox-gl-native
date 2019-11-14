package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import androidx.test.ext.junit.runners.AndroidJUnit4;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.style.types.Formatted;
import com.mapbox.mapboxsdk.style.types.FormattedSection;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.EspressoTest;
import com.mapbox.mapboxsdk.testapp.utils.ResourceUtils;
import com.mapbox.mapboxsdk.testapp.utils.TestingAsyncUtils;
import com.mapbox.mapboxsdk.utils.ColorUtils;

import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.IOException;
import java.util.HashMap;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.expressions.Expression.FormatOption.formatFontScale;
import static com.mapbox.mapboxsdk.style.expressions.Expression.FormatOption.formatTextColor;
import static com.mapbox.mapboxsdk.style.expressions.Expression.FormatOption.formatTextFont;
import static com.mapbox.mapboxsdk.style.expressions.Expression.NumberFormatOption.currency;
import static com.mapbox.mapboxsdk.style.expressions.Expression.NumberFormatOption.locale;
import static com.mapbox.mapboxsdk.style.expressions.Expression.NumberFormatOption.maxFractionDigits;
import static com.mapbox.mapboxsdk.style.expressions.Expression.NumberFormatOption.minFractionDigits;
import static com.mapbox.mapboxsdk.style.expressions.Expression.collator;
import static com.mapbox.mapboxsdk.style.expressions.Expression.color;
import static com.mapbox.mapboxsdk.style.expressions.Expression.eq;
import static com.mapbox.mapboxsdk.style.expressions.Expression.exponential;
import static com.mapbox.mapboxsdk.style.expressions.Expression.format;
import static com.mapbox.mapboxsdk.style.expressions.Expression.formatEntry;
import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.interpolate;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.match;
import static com.mapbox.mapboxsdk.style.expressions.Expression.number;
import static com.mapbox.mapboxsdk.style.expressions.Expression.numberFormat;
import static com.mapbox.mapboxsdk.style.expressions.Expression.rgb;
import static com.mapbox.mapboxsdk.style.expressions.Expression.rgba;
import static com.mapbox.mapboxsdk.style.expressions.Expression.step;
import static com.mapbox.mapboxsdk.style.expressions.Expression.stop;
import static com.mapbox.mapboxsdk.style.expressions.Expression.string;
import static com.mapbox.mapboxsdk.style.expressions.Expression.switchCase;
import static com.mapbox.mapboxsdk.style.expressions.Expression.toColor;
import static com.mapbox.mapboxsdk.style.expressions.Expression.zoom;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillAntialias;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillOutlineColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textField;
import static com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke;
import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;

@RunWith(AndroidJUnit4.class)
public class ExpressionTest extends EspressoTest {

  private FillLayer layer;

  @Test
  public void testConstantExpressionConversion() {
    validateTestSetup();
    setupStyle();
    Timber.i("camera function");

    invoke(mapboxMap, (uiController, mapboxMap) -> {
      // create color expression
      Expression inputExpression = rgba(255.0f, 0.0f, 0.0f, 1.0f);

      // set color expression
      layer.setProperties(
        fillColor(inputExpression)
      );

      // get color value
      int color = layer.getFillColor().getColorInt();

      // compare
      assertEquals("input expression should match", Color.RED, color);
    });
  }

  @Test
  public void testGetExpressionWrapping() {
    validateTestSetup();
    setupStyle();
    Timber.i("camera function");

    invoke(mapboxMap, (uiController, mapboxMap) -> {
      // create get expression
      Expression inputExpression = get("fill");

      // set get expression
      layer.setProperties(
        fillColor(inputExpression)
      );

      // get actual expression
      Expression actualExpression = layer.getFillColor().getExpression();

      // create wrapped expected expression
      Expression expectedExpression = toColor(get("fill"));

      // compare
      assertEquals("input expression should match", expectedExpression, actualExpression);
    });
  }

  @Test
  public void testCameraFunction() {
    validateTestSetup();
    setupStyle();
    Timber.i("camera function");

    invoke(mapboxMap, (uiController, mapboxMap) -> {
      // create camera function expression
      Expression inputExpression = interpolate(
        exponential(0.5f), zoom(),
        stop(1.0f, rgba(255.0f, 0.0f, 0.0f, 1.0f)),
        stop(5.0f, rgba(0.0f, 0.0f, 255.0f, 1.0f)),
        stop(10.0f, rgba(0.0f, 255.0f, 0.0f, 1.0f))
      );

      // set camera function expression
      layer.setProperties(
        fillColor(inputExpression)
      );

      // get camera function expression
      Expression outputExpression = layer.getFillColor().getExpression();

      // compare
      assertEquals("input expression should match", inputExpression, outputExpression);
    });
  }

  @Test
  public void testSourceFunction() {
    validateTestSetup();
    setupStyle();
    Timber.i("camera function");

    invoke(mapboxMap, (uiController, mapboxMap) -> {
      // create camera function expression
      Expression inputExpression = toColor(get("fill"));

      // set camera function expression
      layer.setProperties(
        fillColor(inputExpression)
      );

      // get camera function expression
      Expression outputExpression = layer.getFillColor().getExpression();

      // compare
      assertEquals("input expression should match", inputExpression, outputExpression);
    });
  }

  @Test
  public void testCompositeFunction() {
    validateTestSetup();
    setupStyle();
    Timber.i("camera function");

    invoke(mapboxMap, (uiController, mapboxMap) -> {
      // create camera function expression
      Expression inputExpression = step(zoom(),
        rgba(255.0f, 255.0f, 255.0f, 1.0f),
        stop(7.0f, match(
          string(get("name")),
          literal("Westerpark"), rgba(255.0f, 0.0f, 0.0f, 1.0f),
          rgba(255.0f, 255.0f, 255.0f, 1.0f)
        )),
        stop(8.0f, match(
          string(get("name")),
          literal("Westerpark"), rgba(0.0f, 0.0f, 255.0f, 1.0f),
          rgba(255.0f, 255.0f, 255.0f, 1.0f)
        ))
      );

      // set camera function expression
      layer.setProperties(
        fillColor(inputExpression)
      );

      // get camera function expression
      Expression outputExpression = layer.getFillColor().getExpression();

      // compare
      assertEquals("input expression should match", inputExpression, outputExpression);
    });
  }

  @Test
  public void testLiteralProperty() {
    validateTestSetup();
    setupStyle();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      layer.setProperties(
        fillColor(literal("#4286f4"))
      );
    });
  }

  @Test
  public void testLiteralMatchExpression() {
    validateTestSetup();
    setupStyle();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      Expression expression = match(literal("something"), literal(0f),
        stop("1", get("1")),
        stop("2", get("2")),
        stop("3", get("3")),
        stop("4", get("4"))
      );

      layer.setProperties(
        fillColor(expression)
      );
      expression.toArray();
    });
  }

  @Test
  public void testCollatorExpression() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);

      Expression expression1 = eq(literal("Łukasz"), literal("lukasz"), collator(true, true));
      Expression expression2 = eq(literal("Łukasz"), literal("lukasz"), collator(literal(false), eq(literal(1),
        literal(1)), literal("en")));
      Expression expression3 = eq(literal("Łukasz"), literal("lukasz"), collator(literal(false), eq(literal(2),
        literal(1))));

      mapboxMap.getStyle()
        .addSource(new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude())));
      Layer layer = new CircleLayer("layer", "source")
        .withProperties(circleColor(
          switchCase(
            expression1, literal(ColorUtils.colorToRgbaString(Color.GREEN)),
            literal(ColorUtils.colorToRgbaString(Color.RED))
          )
        ));
      mapboxMap.getStyle().addLayer(layer);
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);
      assertFalse(mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer")
        .isEmpty());

      layer.setProperties(circleColor(
        switchCase(
          expression2, literal(ColorUtils.colorToRgbaString(Color.GREEN)),
          literal(ColorUtils.colorToRgbaString(Color.RED))
        )
      ));
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);
      assertFalse(mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer")
        .isEmpty());

      layer.setProperties(circleColor(
        switchCase(
          expression3, literal(ColorUtils.colorToRgbaString(Color.GREEN)),
          literal(ColorUtils.colorToRgbaString(Color.RED))
        )
      ));
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);
      assertFalse(mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer")
        .isEmpty());
    });
  }

  @Test
  public void testConstFormatExpression() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      mapboxMap.getStyle()
        .addSource(new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude())));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      Expression expression = format(
        formatEntry("test")
      );
      layer.setProperties(textField(expression));
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer")
        .isEmpty());
      assertNull(layer.getTextField().getExpression());
      assertEquals(new Formatted(new FormattedSection("test")), layer.getTextField().getValue());
    });
  }

  @Test
  public void testConstFormatExpressionFontScaleParam() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      mapboxMap.getStyle()
        .addSource(new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude())));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      Expression expression = format(
        formatEntry("test", formatFontScale(1.75))
      );
      layer.setProperties(textField(expression));
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer")
        .isEmpty());
      assertNull(layer.getTextField().getExpression());
      assertEquals(new Formatted(new FormattedSection("test", 1.75)), layer.getTextField().getValue());
    });
  }

  @Test
  public void testConstFormatExpressionTextFontParam() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      mapboxMap.getStyle()
        .addSource(new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude())));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      Expression expression = format(
        formatEntry(
          literal("test"),
          formatTextFont(new String[] {"DIN Offc Pro Regular", "Arial Unicode MS Regular"})
        )
      );
      layer.setProperties(textField(expression));
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(
        mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer").isEmpty()
      );
      assertNull(layer.getTextField().getExpression());
      assertEquals(new Formatted(
        new FormattedSection("test",
          new String[] {"DIN Offc Pro Regular", "Arial Unicode MS Regular"})
      ), layer.getTextField().getValue());
    });
  }

  @Test
  public void testConstFormatExpressionTextColorParam() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      mapboxMap.getStyle()
        .addSource(new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude())));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      Expression expression = format(
        formatEntry(
          literal("test"),
          formatTextColor(literal("yellow"))
        )
      );
      layer.setProperties(textField(expression));
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(
        mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer").isEmpty()
      );
      assertNull(layer.getTextField().getExpression());
      assertEquals(new Formatted(
        new FormattedSection("test", null, null, "rgba(255,255,0,1)")
      ), layer.getTextField().getValue());
    });
  }

  @Test
  public void testConstFormatExpressionAllParams() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      mapboxMap.getStyle()
        .addSource(new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude())));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      Expression expression = format(
        formatEntry(
          "test",
          formatFontScale(0.5),
          formatTextFont(new String[] {"DIN Offc Pro Regular", "Arial Unicode MS Regular"}),
          formatTextColor(rgb(126, 0, 0))
        )
      );
      layer.setProperties(textField(expression));
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(
        mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer").isEmpty()
      );
      assertNull(layer.getTextField().getExpression());
      assertEquals(new Formatted(
        new FormattedSection("test",
          0.5,
          new String[] {"DIN Offc Pro Regular", "Arial Unicode MS Regular"},
          "rgba(126,0,0,1)")
      ), layer.getTextField().getValue());
    });
  }

  @Test
  public void testConstFormatExpressionMultipleInputs() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      mapboxMap.getStyle()
        .addSource(new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude())));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      Expression expression = format(
        formatEntry(
          "test",
          formatFontScale(1.5),
          formatTextFont(new String[] {"DIN Offc Pro Regular", "Arial Unicode MS Regular"})
        ),
        formatEntry("\ntest2", formatFontScale(2), formatTextColor(Color.BLUE)),
        formatEntry("\ntest3", formatFontScale(2.5), formatTextColor(toColor(literal("rgba(0, 128, 255, 0.5)"))))
      );
      layer.setProperties(textField(expression));
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(
        mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer").isEmpty()
      );
      assertNull(layer.getTextField().getExpression());
      assertEquals(new Formatted(
        new FormattedSection("test", 1.5,
          new String[] {"DIN Offc Pro Regular", "Arial Unicode MS Regular"}),
        new FormattedSection("\ntest2", 2.0, null, "rgba(0,0,255,1)"),
        new FormattedSection("\ntest3", 2.5, null, "rgba(0,128,255,0.5)")
      ), layer.getTextField().getValue());
    });
  }

  @Test
  public void testVariableFormatExpression() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      Feature feature = Feature.fromGeometry(Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude()));
      feature.addStringProperty("test_property", "test");
      feature.addNumberProperty("test_property_number", 1.5);
      feature.addStringProperty("test_property_color", "green");
      mapboxMap.getStyle().addSource(new GeoJsonSource("source", feature));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      Expression expression = format(
        formatEntry(
          get("test_property"),
          Expression.FormatOption.formatFontScale(number(get("test_property_number"))),
          formatTextFont(new String[] {"Arial Unicode MS Regular", "DIN Offc Pro Regular"}),
          formatTextColor(toColor(get("test_property_color")))
        )
      );
      layer.setProperties(textField(expression));
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer")
        .isEmpty());
      assertEquals(expression, layer.getTextField().getExpression());
      assertNull(layer.getTextField().getValue());
    });
  }

  @Test
  public void testVariableFormatExpressionMultipleInputs() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      Feature feature = Feature.fromGeometry(Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude()));
      feature.addStringProperty("test_property", "test");
      feature.addNumberProperty("test_property_number", 1.5);
      feature.addStringProperty("test_property_color", "rgba(0, 255, 0, 1)");
      mapboxMap.getStyle().addSource(new GeoJsonSource("source", feature));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      Expression expression = format(
        formatEntry(
          get("test_property"),
          formatFontScale(1.25),
          formatTextFont(new String[] {"Arial Unicode MS Regular", "DIN Offc Pro Regular"}),
          formatTextColor(toColor(get("test_property_color")))
        ),
        formatEntry("\ntest2", formatFontScale(2))
      );
      layer.setProperties(textField(expression), textColor("rgba(128, 0, 0, 1)"));
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer")
        .isEmpty());
      assertEquals(expression, layer.getTextField().getExpression());
      assertNull(layer.getTextField().getValue());
    });
  }

  @Test
  public void testFormatExpressionPlainTextCoercion() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      mapboxMap.getStyle()
        .addSource(new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude())));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      layer.setProperties(textField("test"));
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer")
        .isEmpty());
      assertNull(layer.getTextField().getExpression());
      assertEquals(new Formatted(
        new FormattedSection("test")), layer.getTextField().getValue());
    });
  }

  @Test
  public void testTextFieldFormattedArgument() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      mapboxMap.getStyle()
        .addSource(new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude())));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      Formatted formatted = new Formatted(
        new FormattedSection("test", 1.5),
        new FormattedSection("\ntest", 0.5, new String[] {"Arial Unicode MS Regular", "DIN Offc Pro Regular"}),
        new FormattedSection("test", null, null, "rgba(0,255,0,1)")
      );
      layer.setProperties(textField(formatted), textColor("rgba(128,0,0,1)"));
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer")
        .isEmpty());
      assertNull(layer.getTextField().getExpression());
      assertEquals(formatted, layer.getTextField().getValue());
    });
  }

  @Test
  public void testNumberFormatCurrencyExpression() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      mapboxMap.getStyle()
        .addSource(new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude())));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      layer.setProperties(
        textField(
          numberFormat(12.345, locale("en-US"), currency("USD"))
        )
      );
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(mapboxMap.queryRenderedFeatures(
        mapboxMap.getProjection().toScreenLocation(latLng), "layer").isEmpty()
      );
      assertNull(layer.getTextField().getExpression());
      assertEquals("$12.35", layer.getTextField().getValue().getFormattedSections()[0].getText());
    });
  }

  @Test
  public void testNumberFormatMaxExpression() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      mapboxMap.getStyle()
        .addSource(new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude())));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      layer.setProperties(
        textField(
          numberFormat(12.34567890, maxFractionDigits(5), minFractionDigits(0))
        )
      );
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(mapboxMap.queryRenderedFeatures(
        mapboxMap.getProjection().toScreenLocation(latLng), "layer").isEmpty()
      );
      assertNull(layer.getTextField().getExpression());
      assertEquals("12.34568", layer.getTextField().getValue().getFormattedSections()[0].getText());
    });
  }

  @Test
  public void testNumberFormatMinExpression() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      mapboxMap.getStyle()
        .addSource(new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude())));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      layer.setProperties(
        textField(
          numberFormat(12.0000001, maxFractionDigits(5), minFractionDigits(0))
        )
      );
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(mapboxMap.queryRenderedFeatures(
        mapboxMap.getProjection().toScreenLocation(latLng), "layer").isEmpty()
      );
      assertNull(layer.getTextField().getExpression());
      assertEquals("12", layer.getTextField().getValue().getFormattedSections()[0].getText());
    });
  }

  @Test
  public void testNumberFormatLocaleExpression() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      mapboxMap.getStyle()
        .addSource(new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude())));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      layer.setProperties(
        textField(
          numberFormat(12.0000001, locale("nl-BE"), maxFractionDigits(5), minFractionDigits(1))
        )
      );
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(mapboxMap.queryRenderedFeatures(
        mapboxMap.getProjection().toScreenLocation(latLng), "layer").isEmpty()
      );
      assertNull(layer.getTextField().getExpression());
      assertEquals("12,0", layer.getTextField().getValue().getFormattedSections()[0].getText());
    });
  }

  @Test
  public void testNumberFormatNonConstantExpression() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      Feature feature = Feature.fromGeometry(Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude()));
      feature.addNumberProperty("number_value", 12.345678);
      feature.addStringProperty("locale_value", "nl-BE");
      feature.addNumberProperty("max_value", 5);
      feature.addNumberProperty("min_value", 1);


      mapboxMap.getStyle().addSource(new GeoJsonSource("source", feature));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      Expression numberFormatExpression = numberFormat(
        number(number(get("number_value"))),
        locale(string(get("locale_value"))),
        maxFractionDigits(number(get("max_value"))),
        minFractionDigits(number(get("min_value")))
      );

      layer.setProperties(textField(numberFormatExpression));
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(mapboxMap.queryRenderedFeatures(
        mapboxMap.getProjection().toScreenLocation(latLng), "layer").isEmpty()
      );

      assertNotNull(layer.getTextField().getExpression());

      // Expressions evaluated to string are wrapped by a format expression, take array index 1 to get original
      Object[] returnExpression = (Object[]) layer.getTextField().getExpression().toArray()[1];
      Object[] setExpression = numberFormatExpression.toArray();
      assertEquals("Number format should match",returnExpression[0], setExpression[0]);
      assertArrayEquals("Get value expression should match",
        (Object[]) returnExpression[1],
        (Object[]) setExpression[1]
      );

      // number format objects
      HashMap<String, Object> returnMap = (HashMap<String, Object>) returnExpression[2];
      HashMap<String, Object> setMap = (HashMap<String, Object>) returnExpression[2];

      assertArrayEquals("Number format min fraction digits should match ",
        (Object[]) returnMap.get("min-fraction-digits"),
        (Object[]) setMap.get("min-fraction-digits")
      );

      assertArrayEquals("Number format max fraction digits should match ",
        (Object[]) returnMap.get("max-fraction-digits"),
        (Object[]) setMap.get("max-fraction-digits")
      );

      assertArrayEquals("Number format min fraction digits should match ",
        (Object[]) returnMap.get("locale"),
        (Object[]) setMap.get("locale")
      );
    });

  }

  /**
   * Regression test for #15532
   */
  @Test
  public void testDoubleConversion() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      LatLng latLng = new LatLng(51, 17);
      mapboxMap.getStyle().addSource(
        new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude()))
      );

      CircleLayer layer = new CircleLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      Expression input =  interpolate(
        exponential(0.5f), zoom(),
        stop(-0.1, color(Color.RED)),
        stop(0, color(Color.BLUE))
      );

      layer.setProperties(circleColor(input));

      Expression output = layer.getCircleColor().getExpression();
      assertArrayEquals("Expression should match", input.toArray(), output.toArray());
    });
  }

  private void setupStyle() {
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      // Add a source
      Source source;
      try {
        source = new GeoJsonSource("amsterdam-parks-source",
          ResourceUtils.readRawResource(rule.getActivity(), R.raw.amsterdam));
        mapboxMap.getStyle().addSource(source);
      } catch (IOException ioException) {
        return;
      }

      // Add a fill layer
      mapboxMap.getStyle().addLayer(layer = new FillLayer("amsterdam-parks-layer", source.getId())
        .withProperties(
          fillColor(rgba(0.0f, 0.0f, 0.0f, 0.5f)),
          fillOutlineColor(rgb(0, 0, 255)),
          fillAntialias(true)
        )
      );
    });
  }
}
