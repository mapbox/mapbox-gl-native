package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.support.test.espresso.UiController;
import android.support.test.runner.AndroidJUnit4;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
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
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.ResourceUtils;
import com.mapbox.mapboxsdk.utils.ColorUtils;

import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.IOException;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.expressions.Expression.FormatOption.formatFontScale;
import static com.mapbox.mapboxsdk.style.expressions.Expression.FormatOption.formatTextFont;
import static com.mapbox.mapboxsdk.style.expressions.Expression.collator;
import static com.mapbox.mapboxsdk.style.expressions.Expression.eq;
import static com.mapbox.mapboxsdk.style.expressions.Expression.exponential;
import static com.mapbox.mapboxsdk.style.expressions.Expression.format;
import static com.mapbox.mapboxsdk.style.expressions.Expression.formatEntry;
import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.interpolate;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.match;
import static com.mapbox.mapboxsdk.style.expressions.Expression.number;
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
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textField;
import static com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;

@RunWith(AndroidJUnit4.class)
public class ExpressionTest extends BaseActivityTest {

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
      uiController.loopMainThreadForAtLeast(1000);
      assertFalse(mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer")
        .isEmpty());

      layer.setProperties(circleColor(
        switchCase(
          expression2, literal(ColorUtils.colorToRgbaString(Color.GREEN)),
          literal(ColorUtils.colorToRgbaString(Color.RED))
        )
      ));
      uiController.loopMainThreadForAtLeast(1000);
      assertFalse(mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer")
        .isEmpty());

      layer.setProperties(circleColor(
        switchCase(
          expression3, literal(ColorUtils.colorToRgbaString(Color.GREEN)),
          literal(ColorUtils.colorToRgbaString(Color.RED))
        )
      ));
      uiController.loopMainThreadForAtLeast(1000);
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
      waitForLayer(uiController, mapboxMap, latLng);
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
      waitForLayer(uiController, mapboxMap, latLng);
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
      waitForLayer(uiController, mapboxMap, latLng);
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
          formatTextFont(new String[] {"DIN Offc Pro Regular", "Arial Unicode MS Regular"})
        )
      );
      layer.setProperties(textField(expression));
      waitForLayer(uiController, mapboxMap, latLng);
      assertFalse(
        mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer").isEmpty()
      );

      assertNull(layer.getTextField().getExpression());
      assertEquals(new Formatted(
        new FormattedSection("test",
          0.5,
          new String[] {"DIN Offc Pro Regular", "Arial Unicode MS Regular"})
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
        formatEntry("\ntest2", formatFontScale(2))
      );
      layer.setProperties(textField(expression));
      waitForLayer(uiController, mapboxMap, latLng);
      assertFalse(
        mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer").isEmpty()
      );

      assertNull(layer.getTextField().getExpression());
      assertEquals(new Formatted(
        new FormattedSection("test", 1.5,
          new String[] {"DIN Offc Pro Regular", "Arial Unicode MS Regular"}),
        new FormattedSection("\ntest2", 2.0)
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
      mapboxMap.getStyle().addSource(new GeoJsonSource("source", feature));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      Expression expression = format(
        formatEntry(
          get("test_property"),
          Expression.FormatOption.formatFontScale(number(get("test_property_number"))),
          formatTextFont(new String[] {"Arial Unicode MS Regular", "DIN Offc Pro Regular"})
        )
      );
      layer.setProperties(textField(expression));
      waitForLayer(uiController, mapboxMap, latLng);
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
      mapboxMap.getStyle().addSource(new GeoJsonSource("source", feature));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.getStyle().addLayer(layer);

      Expression expression = format(
        formatEntry(
          get("test_property"),
          formatFontScale(1.25),
          formatTextFont(new String[] {"Arial Unicode MS Regular", "DIN Offc Pro Regular"})
        ),
        formatEntry("\ntest2", formatFontScale(2))
      );
      layer.setProperties(textField(expression));
      waitForLayer(uiController, mapboxMap, latLng);
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
      waitForLayer(uiController, mapboxMap, latLng);
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
      mapboxMap.addSource(new GeoJsonSource("source", Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude())));
      SymbolLayer layer = new SymbolLayer("layer", "source");
      mapboxMap.addLayer(layer);

      Formatted formatted = new Formatted(
        new FormattedSection("test", 1.5),
        new FormattedSection("\ntest", 0.5, new String[] {"Arial Unicode MS Regular", "DIN Offc Pro Regular"})
      );
      layer.setProperties(textField(formatted));
      waitForLayer(uiController, mapboxMap, latLng);
      assertFalse(mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer")
        .isEmpty());

      assertNull(layer.getTextField().getExpression());
      assertEquals(formatted, layer.getTextField().getValue());
    });
  }

  private static final long WAIT_TIMEOUT = 5000;
  private static final long WAIT_DELAY = 150;

  private static void waitForLayer(UiController uiController, MapboxMap mapboxMap, LatLng latLng) {
    int i = 0;
    while (mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), "layer").isEmpty()) {
      i++;
      assertFalse("Waiting for layer timed out", i * WAIT_DELAY > WAIT_TIMEOUT);
      uiController.loopMainThreadForAtLeast(WAIT_DELAY);
    }
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

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }
}
