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
import com.mapbox.mapboxsdk.style.layers.FillExtrusionLayer;
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
 * Basic smoke tests for FillExtrusionLayer
 */
@RunWith(AndroidJUnit4.class)
public class FillExtrusionLayerTest extends BaseStyleTest {

  @Rule
  public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

  private FillExtrusionLayer layer;

  private OnMapReadyIdlingResource idlingResource;

  private MapboxMap mapboxMap;

  @Before
  public void setup() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
    mapboxMap = rule.getActivity().getMapboxMap();

    if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
      Timber.i("Adding layer");
      layer = new FillExtrusionLayer("my-layer", "composite");
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
  public void testFillExtrusionOpacityAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-opacity");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillExtrusionOpacity(0.3f));
    assertEquals((Float) layer.getFillExtrusionOpacity().getValue(), (Float) 0.3f);
  }

  @Test
  public void testFillExtrusionOpacityAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionOpacity(
        zoom(
          exponential(
            stop(2, fillExtrusionOpacity(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionOpacity());
    assertNotNull(layer.getFillExtrusionOpacity().getFunction());
    assertEquals(CameraFunction.class, layer.getFillExtrusionOpacity().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getFillExtrusionOpacity().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getFillExtrusionOpacity().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getFillExtrusionOpacity().getFunction().getStops()).size());
  }

  @Test
  public void testFillExtrusionColorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillExtrusionColor("rgba(0, 0, 0, 1)"));
    assertEquals((String) layer.getFillExtrusionColor().getValue(), (String) "rgba(0, 0, 0, 1)");
  }

  @Test
  public void testFillExtrusionColorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionColor(
        zoom(
          exponential(
            stop(2, fillExtrusionColor("rgba(0, 0, 0, 1)"))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionColor());
    assertNotNull(layer.getFillExtrusionColor().getFunction());
    assertEquals(CameraFunction.class, layer.getFillExtrusionColor().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getFillExtrusionColor().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getFillExtrusionColor().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getFillExtrusionColor().getFunction().getStops()).size());
  }

  @Test
  public void testFillExtrusionColorAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionColor(property("FeaturePropertyA", Stops.<String>identity()))
    );

    // Verify
    assertNotNull(layer.getFillExtrusionColor());
    assertNotNull(layer.getFillExtrusionColor().getFunction());
    assertEquals(SourceFunction.class, layer.getFillExtrusionColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillExtrusionColor().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getFillExtrusionColor().getFunction().getStops().getClass());
  }

  @Test
  public void testFillExtrusionColorAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionColor(
        property(
          "FeaturePropertyA",
          exponential(
            stop(Color.RED, fillExtrusionColor(Color.RED))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionColor());
    assertNotNull(layer.getFillExtrusionColor().getFunction());
    assertEquals(SourceFunction.class, layer.getFillExtrusionColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillExtrusionColor().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getFillExtrusionColor().getFunction().getStops().getClass());
  }

  @Test
  public void testFillExtrusionColorAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionColor(
        property(
          "FeaturePropertyA",
          categorical(
            stop("valueA", fillExtrusionColor(Color.RED))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionColor());
    assertNotNull(layer.getFillExtrusionColor().getFunction());
    assertEquals(SourceFunction.class, layer.getFillExtrusionColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillExtrusionColor().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getFillExtrusionColor().getFunction().getStops().getClass());
  }

  @Test
  public void testFillExtrusionColorAsIntConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillExtrusionColor(Color.RED));
    assertEquals(layer.getFillExtrusionColorAsInt(), Color.RED);
  }

  @Test
  public void testFillExtrusionTranslateAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-translate");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillExtrusionTranslate(new Float[]{0f,0f}));
    assertEquals((Float[]) layer.getFillExtrusionTranslate().getValue(), (Float[]) new Float[]{0f,0f});
  }

  @Test
  public void testFillExtrusionTranslateAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-translate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionTranslate(
        zoom(
          exponential(
            stop(2, fillExtrusionTranslate(new Float[]{0f,0f}))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionTranslate());
    assertNotNull(layer.getFillExtrusionTranslate().getFunction());
    assertEquals(CameraFunction.class, layer.getFillExtrusionTranslate().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getFillExtrusionTranslate().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getFillExtrusionTranslate().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getFillExtrusionTranslate().getFunction().getStops()).size());
  }

  @Test
  public void testFillExtrusionTranslateAnchorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-translate-anchor");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillExtrusionTranslateAnchor(FILL_EXTRUSION_TRANSLATE_ANCHOR_MAP));
    assertEquals((String) layer.getFillExtrusionTranslateAnchor().getValue(), (String) FILL_EXTRUSION_TRANSLATE_ANCHOR_MAP);
  }

  @Test
  public void testFillExtrusionTranslateAnchorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-translate-anchor");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionTranslateAnchor(
        zoom(
          interval(
            stop(2, fillExtrusionTranslateAnchor(FILL_EXTRUSION_TRANSLATE_ANCHOR_MAP))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionTranslateAnchor());
    assertNotNull(layer.getFillExtrusionTranslateAnchor().getFunction());
    assertEquals(CameraFunction.class, layer.getFillExtrusionTranslateAnchor().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getFillExtrusionTranslateAnchor().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getFillExtrusionTranslateAnchor().getFunction().getStops()).size());
  }

  @Test
  public void testFillExtrusionPatternAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-pattern");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillExtrusionPattern("pedestrian-polygon"));
    assertEquals((String) layer.getFillExtrusionPattern().getValue(), (String) "pedestrian-polygon");
  }

  @Test
  public void testFillExtrusionPatternAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-pattern");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionPattern(
        zoom(
          interval(
            stop(2, fillExtrusionPattern("pedestrian-polygon"))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionPattern());
    assertNotNull(layer.getFillExtrusionPattern().getFunction());
    assertEquals(CameraFunction.class, layer.getFillExtrusionPattern().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getFillExtrusionPattern().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getFillExtrusionPattern().getFunction().getStops()).size());
  }

  @Test
  public void testFillExtrusionHeightAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-height");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillExtrusionHeight(0.3f));
    assertEquals((Float) layer.getFillExtrusionHeight().getValue(), (Float) 0.3f);
  }

  @Test
  public void testFillExtrusionHeightAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-height");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionHeight(
        zoom(
          exponential(
            stop(2, fillExtrusionHeight(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionHeight());
    assertNotNull(layer.getFillExtrusionHeight().getFunction());
    assertEquals(CameraFunction.class, layer.getFillExtrusionHeight().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getFillExtrusionHeight().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getFillExtrusionHeight().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getFillExtrusionHeight().getFunction().getStops()).size());
  }

  @Test
  public void testFillExtrusionHeightAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-height");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionHeight(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getFillExtrusionHeight());
    assertNotNull(layer.getFillExtrusionHeight().getFunction());
    assertEquals(SourceFunction.class, layer.getFillExtrusionHeight().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillExtrusionHeight().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getFillExtrusionHeight().getFunction().getStops().getClass());
  }

  @Test
  public void testFillExtrusionHeightAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-height");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionHeight(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, fillExtrusionHeight(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionHeight());
    assertNotNull(layer.getFillExtrusionHeight().getFunction());
    assertEquals(SourceFunction.class, layer.getFillExtrusionHeight().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillExtrusionHeight().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getFillExtrusionHeight().getFunction().getStops().getClass());
  }

  @Test
  public void testFillExtrusionHeightAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-height");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionHeight(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, fillExtrusionHeight(0.3f))
          )
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionHeight());
    assertNotNull(layer.getFillExtrusionHeight().getFunction());
    assertEquals(SourceFunction.class, layer.getFillExtrusionHeight().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillExtrusionHeight().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getFillExtrusionHeight().getFunction().getStops().getClass());
    assertEquals(0.3f, ((SourceFunction) layer.getFillExtrusionHeight().getFunction()).getDefaultValue());
  }

  @Test
  public void testFillExtrusionHeightAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-height");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionHeight(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, fillExtrusionHeight(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionHeight());
    assertNotNull(layer.getFillExtrusionHeight().getFunction());
    assertEquals(CompositeFunction.class, layer.getFillExtrusionHeight().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getFillExtrusionHeight().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getFillExtrusionHeight().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getFillExtrusionHeight().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getFillExtrusionHeight().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testFillExtrusionBaseAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-base");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(fillExtrusionBase(0.3f));
    assertEquals((Float) layer.getFillExtrusionBase().getValue(), (Float) 0.3f);
  }

  @Test
  public void testFillExtrusionBaseAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-base");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionBase(
        zoom(
          exponential(
            stop(2, fillExtrusionBase(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionBase());
    assertNotNull(layer.getFillExtrusionBase().getFunction());
    assertEquals(CameraFunction.class, layer.getFillExtrusionBase().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getFillExtrusionBase().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getFillExtrusionBase().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getFillExtrusionBase().getFunction().getStops()).size());
  }

  @Test
  public void testFillExtrusionBaseAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-base");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionBase(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getFillExtrusionBase());
    assertNotNull(layer.getFillExtrusionBase().getFunction());
    assertEquals(SourceFunction.class, layer.getFillExtrusionBase().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillExtrusionBase().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getFillExtrusionBase().getFunction().getStops().getClass());
  }

  @Test
  public void testFillExtrusionBaseAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-base");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionBase(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, fillExtrusionBase(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionBase());
    assertNotNull(layer.getFillExtrusionBase().getFunction());
    assertEquals(SourceFunction.class, layer.getFillExtrusionBase().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillExtrusionBase().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getFillExtrusionBase().getFunction().getStops().getClass());
  }

  @Test
  public void testFillExtrusionBaseAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-base");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionBase(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, fillExtrusionBase(0.3f))
          )
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionBase());
    assertNotNull(layer.getFillExtrusionBase().getFunction());
    assertEquals(SourceFunction.class, layer.getFillExtrusionBase().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillExtrusionBase().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getFillExtrusionBase().getFunction().getStops().getClass());
    assertEquals(0.3f, ((SourceFunction) layer.getFillExtrusionBase().getFunction()).getDefaultValue());
  }

  @Test
  public void testFillExtrusionBaseAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("fill-extrusion-base");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      fillExtrusionBase(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, fillExtrusionBase(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getFillExtrusionBase());
    assertNotNull(layer.getFillExtrusionBase().getFunction());
    assertEquals(CompositeFunction.class, layer.getFillExtrusionBase().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getFillExtrusionBase().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getFillExtrusionBase().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getFillExtrusionBase().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getFillExtrusionBase().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }


  @After
  public void unregisterIntentServiceIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }
}
