// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.support.test.espresso.Espresso;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import timber.log.Timber;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.functions.CompositeFunction;
import com.mapbox.mapboxsdk.style.functions.CameraFunction;
import com.mapbox.mapboxsdk.style.functions.SourceFunction;
import com.mapbox.mapboxsdk.style.functions.stops.CategoricalStops;
import com.mapbox.mapboxsdk.style.functions.stops.ExponentialStops;
import com.mapbox.mapboxsdk.style.functions.stops.IdentityStops;
import com.mapbox.mapboxsdk.style.functions.stops.IntervalStops;
import com.mapbox.mapboxsdk.style.functions.stops.Stop;
import com.mapbox.mapboxsdk.style.functions.stops.Stops;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import static com.mapbox.mapboxsdk.style.functions.Function.*;
import static com.mapbox.mapboxsdk.style.functions.stops.Stop.stop;
import static com.mapbox.mapboxsdk.style.functions.stops.Stops.*;
import static org.junit.Assert.*;
import static com.mapbox.mapboxsdk.style.layers.Property.*;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.*;

/**
 * Basic smoke tests for FillLayer
 */
@RunWith(AndroidJUnit4.class)
public class FillLayerTest extends BaseStyleTest {

  @Rule
  public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

  private FillLayer layer;

  private OnMapReadyIdlingResource idlingResource;

  private MapboxMap mapboxMap;

  @Before
  public void setup() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
    mapboxMap = rule.getActivity().getMapboxMap();

    if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
      Timber.i("Adding layer");
      layer = new FillLayer("my-layer", "composite");
      layer.setSourceLayer("composite");
      mapboxMap.addLayer(layer);
      // Layer reference is now stale, get new reference
      layer = mapboxMap.getLayerAs("my-layer");
    }
  }

  @Test
  public void testSetVisibility() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("Visibility");
    assertNotNull(layer);

    // Get initial
    assertEquals(layer.getVisibility().getValue(), VISIBLE);

    // Set
    layer.setProperties(visibility(NONE));
    assertEquals(layer.getVisibility().getValue(), NONE);
  }

  @Test
  public void testFillAntialiasAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-antialias");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillAntialias(true));
    assertEquals((Boolean) layer.getFillAntialias().getValue(), (Boolean) true);
  }

  @Test
  public void testFillAntialiasAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-antialias");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillAntialias(
        zoom(
          interval(
            stop(2, fillAntialias(true))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillAntialias());
    assertNotNull(layer.getFillAntialias().getFunction());
    assertEquals(CameraFunction.class, layer.getFillAntialias().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getFillAntialias().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getFillAntialias().getFunction().getStops()).size());
  }

  @Test
  public void testFillOpacityAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-opacity");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillOpacity(0.3f));
    assertEquals((Float) layer.getFillOpacity().getValue(), (Float) 0.3f);
  }

  @Test
  public void testFillOpacityAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillOpacity(
        zoom(
          exponential(
            stop(2, fillOpacity(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillOpacity());
    assertNotNull(layer.getFillOpacity().getFunction());
    assertEquals(CameraFunction.class, layer.getFillOpacity().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getFillOpacity().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getFillOpacity().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getFillOpacity().getFunction().getStops()).size());
  }

  @Test
  public void testFillOpacityAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillOpacity(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getFillOpacity());
    assertNotNull(layer.getFillOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getFillOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillOpacity().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getFillOpacity().getFunction().getStops().getClass());
  }

  @Test
  public void testFillOpacityAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillOpacity(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, fillOpacity(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillOpacity());
    assertNotNull(layer.getFillOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getFillOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillOpacity().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getFillOpacity().getFunction().getStops().getClass());
  }

  @Test
  public void testFillOpacityAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillOpacity(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, fillOpacity(0.3f))
          )
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getFillOpacity());
    assertNotNull(layer.getFillOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getFillOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillOpacity().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getFillOpacity().getFunction().getStops().getClass());
    assertEquals(0.3f, ((SourceFunction) layer.getFillOpacity().getFunction()).getDefaultValue());
  }

  @Test
  public void testFillOpacityAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillOpacity(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, fillOpacity(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getFillOpacity());
    assertNotNull(layer.getFillOpacity().getFunction());
    assertEquals(CompositeFunction.class, layer.getFillOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getFillOpacity().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getFillOpacity().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getFillOpacity().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getFillOpacity().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testFillColorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillColor("rgba(0, 0, 0, 1)"));
    assertEquals((String) layer.getFillColor().getValue(), (String) "rgba(0, 0, 0, 1)");
  }

  @Test
  public void testFillColorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillColor(
        zoom(
          exponential(
            stop(2, fillColor("rgba(0, 0, 0, 1)"))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillColor());
    assertNotNull(layer.getFillColor().getFunction());
    assertEquals(CameraFunction.class, layer.getFillColor().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getFillColor().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getFillColor().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getFillColor().getFunction().getStops()).size());
  }

  @Test
  public void testFillColorAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillColor(property("FeaturePropertyA", Stops.<String>identity()))
    );

    // Verify
    assertNotNull(layer.getFillColor());
    assertNotNull(layer.getFillColor().getFunction());
    assertEquals(SourceFunction.class, layer.getFillColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillColor().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getFillColor().getFunction().getStops().getClass());
  }

  @Test
  public void testFillColorAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillColor(
        property(
          "FeaturePropertyA",
          exponential(
            stop(Color.RED, fillColor(Color.RED))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillColor());
    assertNotNull(layer.getFillColor().getFunction());
    assertEquals(SourceFunction.class, layer.getFillColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillColor().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getFillColor().getFunction().getStops().getClass());
  }

  @Test
  public void testFillColorAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillColor(
        property(
          "FeaturePropertyA",
          categorical(
            stop("valueA", fillColor(Color.RED))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillColor());
    assertNotNull(layer.getFillColor().getFunction());
    assertEquals(SourceFunction.class, layer.getFillColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillColor().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getFillColor().getFunction().getStops().getClass());
  }

  @Test
  public void testFillColorAsIntConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillColor(Color.RED));
    assertEquals(layer.getFillColorAsInt(), Color.RED);
  }

  @Test
  public void testFillOutlineColorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-outline-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillOutlineColor("rgba(0, 0, 0, 1)"));
    assertEquals((String) layer.getFillOutlineColor().getValue(), (String) "rgba(0, 0, 0, 1)");
  }

  @Test
  public void testFillOutlineColorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-outline-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillOutlineColor(
        zoom(
          exponential(
            stop(2, fillOutlineColor("rgba(0, 0, 0, 1)"))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillOutlineColor());
    assertNotNull(layer.getFillOutlineColor().getFunction());
    assertEquals(CameraFunction.class, layer.getFillOutlineColor().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getFillOutlineColor().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getFillOutlineColor().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getFillOutlineColor().getFunction().getStops()).size());
  }

  @Test
  public void testFillOutlineColorAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-outline-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillOutlineColor(property("FeaturePropertyA", Stops.<String>identity()))
    );

    // Verify
    assertNotNull(layer.getFillOutlineColor());
    assertNotNull(layer.getFillOutlineColor().getFunction());
    assertEquals(SourceFunction.class, layer.getFillOutlineColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillOutlineColor().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getFillOutlineColor().getFunction().getStops().getClass());
  }

  @Test
  public void testFillOutlineColorAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-outline-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillOutlineColor(
        property(
          "FeaturePropertyA",
          exponential(
            stop(Color.RED, fillOutlineColor(Color.RED))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillOutlineColor());
    assertNotNull(layer.getFillOutlineColor().getFunction());
    assertEquals(SourceFunction.class, layer.getFillOutlineColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillOutlineColor().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getFillOutlineColor().getFunction().getStops().getClass());
  }

  @Test
  public void testFillOutlineColorAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-outline-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillOutlineColor(
        property(
          "FeaturePropertyA",
          categorical(
            stop("valueA", fillOutlineColor(Color.RED))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillOutlineColor());
    assertNotNull(layer.getFillOutlineColor().getFunction());
    assertEquals(SourceFunction.class, layer.getFillOutlineColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillOutlineColor().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getFillOutlineColor().getFunction().getStops().getClass());
  }

  @Test
  public void testFillOutlineColorAsIntConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-outline-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillOutlineColor(Color.RED));
    assertEquals(layer.getFillOutlineColorAsInt(), Color.RED);
  }

  @Test
  public void testFillTranslateAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-translate");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillTranslate(new Float[]{0f,0f}));
    assertEquals((Float[]) layer.getFillTranslate().getValue(), (Float[]) new Float[]{0f,0f});
  }

  @Test
  public void testFillTranslateAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-translate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillTranslate(
        zoom(
          exponential(
            stop(2, fillTranslate(new Float[]{0f,0f}))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillTranslate());
    assertNotNull(layer.getFillTranslate().getFunction());
    assertEquals(CameraFunction.class, layer.getFillTranslate().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getFillTranslate().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getFillTranslate().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getFillTranslate().getFunction().getStops()).size());
  }

  @Test
  public void testFillTranslateAnchorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-translate-anchor");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillTranslateAnchor(FILL_TRANSLATE_ANCHOR_MAP));
    assertEquals((String) layer.getFillTranslateAnchor().getValue(), (String) FILL_TRANSLATE_ANCHOR_MAP);
  }

  @Test
  public void testFillTranslateAnchorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-translate-anchor");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillTranslateAnchor(
        zoom(
          interval(
            stop(2, fillTranslateAnchor(FILL_TRANSLATE_ANCHOR_MAP))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillTranslateAnchor());
    assertNotNull(layer.getFillTranslateAnchor().getFunction());
    assertEquals(CameraFunction.class, layer.getFillTranslateAnchor().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getFillTranslateAnchor().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getFillTranslateAnchor().getFunction().getStops()).size());
  }

  @Test
  public void testFillPatternAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-pattern");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillPattern("pedestrian-polygon"));
    assertEquals((String) layer.getFillPattern().getValue(), (String) "pedestrian-polygon");
  }

  @Test
  public void testFillPatternAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-pattern");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillPattern(
        zoom(
          interval(
            stop(2, fillPattern("pedestrian-polygon"))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillPattern());
    assertNotNull(layer.getFillPattern().getFunction());
    assertEquals(CameraFunction.class, layer.getFillPattern().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getFillPattern().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getFillPattern().getFunction().getStops()).size());
  }


  @After
  public void unregisterIntentServiceIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }
}
