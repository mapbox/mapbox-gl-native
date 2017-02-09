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
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
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
 * Basic smoke tests for CircleLayer
 */
@RunWith(AndroidJUnit4.class)
public class CircleLayerTest extends BaseStyleTest {

  @Rule
  public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

  private CircleLayer layer;

  private OnMapReadyIdlingResource idlingResource;

  private MapboxMap mapboxMap;

  @Before
  public void setup() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
    mapboxMap = rule.getActivity().getMapboxMap();

    if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
      Timber.i("Adding layer");
      layer = new CircleLayer("my-layer", "composite");
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
  public void testCircleRadiusAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-radius");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(circleRadius(0.3f));
    assertEquals((Float) layer.getCircleRadius().getValue(), (Float) 0.3f);
  }

  @Test
  public void testCircleRadiusAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-radius");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleRadius(
        zoom(
          exponential(
            stop(2, circleRadius(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleRadius());
    assertNotNull(layer.getCircleRadius().getFunction());
    assertEquals(CameraFunction.class, layer.getCircleRadius().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getCircleRadius().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getCircleRadius().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getCircleRadius().getFunction().getStops()).size());
  }

  @Test
  public void testCircleRadiusAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-radius");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleRadius(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getCircleRadius());
    assertNotNull(layer.getCircleRadius().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleRadius().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleRadius().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getCircleRadius().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleRadiusAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-radius");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleRadius(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, circleRadius(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleRadius());
    assertNotNull(layer.getCircleRadius().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleRadius().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleRadius().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getCircleRadius().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleRadiusAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-radius");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleRadius(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, circleRadius(0.3f))
          )
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getCircleRadius());
    assertNotNull(layer.getCircleRadius().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleRadius().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleRadius().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getCircleRadius().getFunction().getStops().getClass());
    assertEquals(0.3f, ((SourceFunction) layer.getCircleRadius().getFunction()).getDefaultValue());
  }

  @Test
  public void testCircleRadiusAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-radius");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleRadius(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, circleRadius(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getCircleRadius());
    assertNotNull(layer.getCircleRadius().getFunction());
    assertEquals(CompositeFunction.class, layer.getCircleRadius().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getCircleRadius().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getCircleRadius().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getCircleRadius().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getCircleRadius().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testCircleColorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(circleColor("rgba(0, 0, 0, 1)"));
    assertEquals((String) layer.getCircleColor().getValue(), (String) "rgba(0, 0, 0, 1)");
  }

  @Test
  public void testCircleColorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleColor(
        zoom(
          exponential(
            stop(2, circleColor("rgba(0, 0, 0, 1)"))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleColor());
    assertNotNull(layer.getCircleColor().getFunction());
    assertEquals(CameraFunction.class, layer.getCircleColor().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getCircleColor().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getCircleColor().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getCircleColor().getFunction().getStops()).size());
  }

  @Test
  public void testCircleColorAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleColor(property("FeaturePropertyA", Stops.<String>identity()))
    );

    // Verify
    assertNotNull(layer.getCircleColor());
    assertNotNull(layer.getCircleColor().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleColor().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getCircleColor().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleColorAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleColor(
        property(
          "FeaturePropertyA",
          exponential(
            stop(Color.RED, circleColor(Color.RED))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleColor());
    assertNotNull(layer.getCircleColor().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleColor().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getCircleColor().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleColorAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleColor(
        property(
          "FeaturePropertyA",
          categorical(
            stop("valueA", circleColor(Color.RED))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleColor());
    assertNotNull(layer.getCircleColor().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleColor().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getCircleColor().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleColorAsIntConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(circleColor(Color.RED));
    assertEquals(layer.getCircleColorAsInt(), Color.RED);
  }

  @Test
  public void testCircleBlurAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-blur");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(circleBlur(0.3f));
    assertEquals((Float) layer.getCircleBlur().getValue(), (Float) 0.3f);
  }

  @Test
  public void testCircleBlurAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleBlur(
        zoom(
          exponential(
            stop(2, circleBlur(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleBlur());
    assertNotNull(layer.getCircleBlur().getFunction());
    assertEquals(CameraFunction.class, layer.getCircleBlur().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getCircleBlur().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getCircleBlur().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getCircleBlur().getFunction().getStops()).size());
  }

  @Test
  public void testCircleBlurAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleBlur(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getCircleBlur());
    assertNotNull(layer.getCircleBlur().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleBlur().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleBlur().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getCircleBlur().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleBlurAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleBlur(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, circleBlur(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleBlur());
    assertNotNull(layer.getCircleBlur().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleBlur().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleBlur().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getCircleBlur().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleBlurAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleBlur(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, circleBlur(0.3f))
          )
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getCircleBlur());
    assertNotNull(layer.getCircleBlur().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleBlur().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleBlur().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getCircleBlur().getFunction().getStops().getClass());
    assertEquals(0.3f, ((SourceFunction) layer.getCircleBlur().getFunction()).getDefaultValue());
  }

  @Test
  public void testCircleBlurAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleBlur(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, circleBlur(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getCircleBlur());
    assertNotNull(layer.getCircleBlur().getFunction());
    assertEquals(CompositeFunction.class, layer.getCircleBlur().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getCircleBlur().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getCircleBlur().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getCircleBlur().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getCircleBlur().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testCircleOpacityAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-opacity");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(circleOpacity(0.3f));
    assertEquals((Float) layer.getCircleOpacity().getValue(), (Float) 0.3f);
  }

  @Test
  public void testCircleOpacityAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleOpacity(
        zoom(
          exponential(
            stop(2, circleOpacity(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleOpacity());
    assertNotNull(layer.getCircleOpacity().getFunction());
    assertEquals(CameraFunction.class, layer.getCircleOpacity().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getCircleOpacity().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getCircleOpacity().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getCircleOpacity().getFunction().getStops()).size());
  }

  @Test
  public void testCircleOpacityAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleOpacity(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getCircleOpacity());
    assertNotNull(layer.getCircleOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleOpacity().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getCircleOpacity().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleOpacityAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleOpacity(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, circleOpacity(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleOpacity());
    assertNotNull(layer.getCircleOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleOpacity().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getCircleOpacity().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleOpacityAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleOpacity(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, circleOpacity(0.3f))
          )
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getCircleOpacity());
    assertNotNull(layer.getCircleOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleOpacity().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getCircleOpacity().getFunction().getStops().getClass());
    assertEquals(0.3f, ((SourceFunction) layer.getCircleOpacity().getFunction()).getDefaultValue());
  }

  @Test
  public void testCircleOpacityAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleOpacity(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, circleOpacity(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getCircleOpacity());
    assertNotNull(layer.getCircleOpacity().getFunction());
    assertEquals(CompositeFunction.class, layer.getCircleOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getCircleOpacity().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getCircleOpacity().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getCircleOpacity().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getCircleOpacity().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testCircleTranslateAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-translate");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(circleTranslate(new Float[]{0f,0f}));
    assertEquals((Float[]) layer.getCircleTranslate().getValue(), (Float[]) new Float[]{0f,0f});
  }

  @Test
  public void testCircleTranslateAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-translate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleTranslate(
        zoom(
          exponential(
            stop(2, circleTranslate(new Float[]{0f,0f}))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleTranslate());
    assertNotNull(layer.getCircleTranslate().getFunction());
    assertEquals(CameraFunction.class, layer.getCircleTranslate().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getCircleTranslate().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getCircleTranslate().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getCircleTranslate().getFunction().getStops()).size());
  }

  @Test
  public void testCircleTranslateAnchorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-translate-anchor");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(circleTranslateAnchor(CIRCLE_TRANSLATE_ANCHOR_MAP));
    assertEquals((String) layer.getCircleTranslateAnchor().getValue(), (String) CIRCLE_TRANSLATE_ANCHOR_MAP);
  }

  @Test
  public void testCircleTranslateAnchorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-translate-anchor");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleTranslateAnchor(
        zoom(
          interval(
            stop(2, circleTranslateAnchor(CIRCLE_TRANSLATE_ANCHOR_MAP))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleTranslateAnchor());
    assertNotNull(layer.getCircleTranslateAnchor().getFunction());
    assertEquals(CameraFunction.class, layer.getCircleTranslateAnchor().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getCircleTranslateAnchor().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getCircleTranslateAnchor().getFunction().getStops()).size());
  }

  @Test
  public void testCirclePitchScaleAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-pitch-scale");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(circlePitchScale(CIRCLE_PITCH_SCALE_MAP));
    assertEquals((String) layer.getCirclePitchScale().getValue(), (String) CIRCLE_PITCH_SCALE_MAP);
  }

  @Test
  public void testCirclePitchScaleAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-pitch-scale");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circlePitchScale(
        zoom(
          interval(
            stop(2, circlePitchScale(CIRCLE_PITCH_SCALE_MAP))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getCirclePitchScale());
    assertNotNull(layer.getCirclePitchScale().getFunction());
    assertEquals(CameraFunction.class, layer.getCirclePitchScale().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getCirclePitchScale().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getCirclePitchScale().getFunction().getStops()).size());
  }

  @Test
  public void testCircleStrokeWidthAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-width");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(circleStrokeWidth(0.3f));
    assertEquals((Float) layer.getCircleStrokeWidth().getValue(), (Float) 0.3f);
  }

  @Test
  public void testCircleStrokeWidthAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleStrokeWidth(
        zoom(
          exponential(
            stop(2, circleStrokeWidth(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleStrokeWidth());
    assertNotNull(layer.getCircleStrokeWidth().getFunction());
    assertEquals(CameraFunction.class, layer.getCircleStrokeWidth().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getCircleStrokeWidth().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getCircleStrokeWidth().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getCircleStrokeWidth().getFunction().getStops()).size());
  }

  @Test
  public void testCircleStrokeWidthAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleStrokeWidth(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getCircleStrokeWidth());
    assertNotNull(layer.getCircleStrokeWidth().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleStrokeWidth().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleStrokeWidth().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getCircleStrokeWidth().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleStrokeWidthAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleStrokeWidth(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, circleStrokeWidth(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleStrokeWidth());
    assertNotNull(layer.getCircleStrokeWidth().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleStrokeWidth().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleStrokeWidth().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getCircleStrokeWidth().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleStrokeWidthAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleStrokeWidth(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, circleStrokeWidth(0.3f))
          )
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getCircleStrokeWidth());
    assertNotNull(layer.getCircleStrokeWidth().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleStrokeWidth().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleStrokeWidth().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getCircleStrokeWidth().getFunction().getStops().getClass());
    assertEquals(0.3f, ((SourceFunction) layer.getCircleStrokeWidth().getFunction()).getDefaultValue());
  }

  @Test
  public void testCircleStrokeWidthAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleStrokeWidth(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, circleStrokeWidth(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getCircleStrokeWidth());
    assertNotNull(layer.getCircleStrokeWidth().getFunction());
    assertEquals(CompositeFunction.class, layer.getCircleStrokeWidth().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getCircleStrokeWidth().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getCircleStrokeWidth().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getCircleStrokeWidth().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getCircleStrokeWidth().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testCircleStrokeColorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(circleStrokeColor("rgba(0, 0, 0, 1)"));
    assertEquals((String) layer.getCircleStrokeColor().getValue(), (String) "rgba(0, 0, 0, 1)");
  }

  @Test
  public void testCircleStrokeColorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleStrokeColor(
        zoom(
          exponential(
            stop(2, circleStrokeColor("rgba(0, 0, 0, 1)"))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleStrokeColor());
    assertNotNull(layer.getCircleStrokeColor().getFunction());
    assertEquals(CameraFunction.class, layer.getCircleStrokeColor().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getCircleStrokeColor().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getCircleStrokeColor().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getCircleStrokeColor().getFunction().getStops()).size());
  }

  @Test
  public void testCircleStrokeColorAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleStrokeColor(property("FeaturePropertyA", Stops.<String>identity()))
    );

    // Verify
    assertNotNull(layer.getCircleStrokeColor());
    assertNotNull(layer.getCircleStrokeColor().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleStrokeColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleStrokeColor().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getCircleStrokeColor().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleStrokeColorAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleStrokeColor(
        property(
          "FeaturePropertyA",
          exponential(
            stop(Color.RED, circleStrokeColor(Color.RED))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleStrokeColor());
    assertNotNull(layer.getCircleStrokeColor().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleStrokeColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleStrokeColor().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getCircleStrokeColor().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleStrokeColorAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleStrokeColor(
        property(
          "FeaturePropertyA",
          categorical(
            stop("valueA", circleStrokeColor(Color.RED))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleStrokeColor());
    assertNotNull(layer.getCircleStrokeColor().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleStrokeColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleStrokeColor().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getCircleStrokeColor().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleStrokeColorAsIntConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(circleStrokeColor(Color.RED));
    assertEquals(layer.getCircleStrokeColorAsInt(), Color.RED);
  }

  @Test
  public void testCircleStrokeOpacityAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-opacity");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(circleStrokeOpacity(0.3f));
    assertEquals((Float) layer.getCircleStrokeOpacity().getValue(), (Float) 0.3f);
  }

  @Test
  public void testCircleStrokeOpacityAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleStrokeOpacity(
        zoom(
          exponential(
            stop(2, circleStrokeOpacity(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleStrokeOpacity());
    assertNotNull(layer.getCircleStrokeOpacity().getFunction());
    assertEquals(CameraFunction.class, layer.getCircleStrokeOpacity().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getCircleStrokeOpacity().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getCircleStrokeOpacity().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getCircleStrokeOpacity().getFunction().getStops()).size());
  }

  @Test
  public void testCircleStrokeOpacityAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleStrokeOpacity(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getCircleStrokeOpacity());
    assertNotNull(layer.getCircleStrokeOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleStrokeOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleStrokeOpacity().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getCircleStrokeOpacity().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleStrokeOpacityAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleStrokeOpacity(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, circleStrokeOpacity(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getCircleStrokeOpacity());
    assertNotNull(layer.getCircleStrokeOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleStrokeOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleStrokeOpacity().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getCircleStrokeOpacity().getFunction().getStops().getClass());
  }

  @Test
  public void testCircleStrokeOpacityAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleStrokeOpacity(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, circleStrokeOpacity(0.3f))
          )
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getCircleStrokeOpacity());
    assertNotNull(layer.getCircleStrokeOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getCircleStrokeOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getCircleStrokeOpacity().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getCircleStrokeOpacity().getFunction().getStops().getClass());
    assertEquals(0.3f, ((SourceFunction) layer.getCircleStrokeOpacity().getFunction()).getDefaultValue());
  }

  @Test
  public void testCircleStrokeOpacityAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("circle-stroke-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      circleStrokeOpacity(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, circleStrokeOpacity(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(0.3f)
      )
    );

    // Verify
    assertNotNull(layer.getCircleStrokeOpacity());
    assertNotNull(layer.getCircleStrokeOpacity().getFunction());
    assertEquals(CompositeFunction.class, layer.getCircleStrokeOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getCircleStrokeOpacity().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getCircleStrokeOpacity().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getCircleStrokeOpacity().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getCircleStrokeOpacity().getFunction().getStops();
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
