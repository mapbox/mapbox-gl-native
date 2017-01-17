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
import com.mapbox.mapboxsdk.style.functions.stops.CompositeStops;
import com.mapbox.mapboxsdk.style.functions.stops.ExponentialStops;
import com.mapbox.mapboxsdk.style.functions.stops.IdentityStops;
import com.mapbox.mapboxsdk.style.functions.stops.IntervalStops;
import com.mapbox.mapboxsdk.style.functions.stops.Stops;
import com.mapbox.mapboxsdk.style.layers.BackgroundLayer;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.Map;

import static com.mapbox.mapboxsdk.style.functions.Function.*;
import static com.mapbox.mapboxsdk.style.functions.stops.Stop.stop;
import static com.mapbox.mapboxsdk.style.functions.stops.Stops.*;
import static org.junit.Assert.*;
import static com.mapbox.mapboxsdk.style.layers.Property.*;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.*;

/**
 * Basic smoke tests for BackgroundLayer
 */
@RunWith(AndroidJUnit4.class)
public class BackgroundLayerTest extends BaseStyleTest {

  @Rule
  public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

  private BackgroundLayer layer;

  private OnMapReadyIdlingResource idlingResource;

  private MapboxMap mapboxMap;

  @Before
  public void setup() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
    mapboxMap = rule.getActivity().getMapboxMap();

    Timber.i("Retrieving layer");
    layer = mapboxMap.getLayerAs("background");
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
  public void testBackgroundColorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("background-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(backgroundColor("rgba(0, 0, 0, 1)"));
    assertEquals((String) layer.getBackgroundColor().getValue(), (String) "rgba(0, 0, 0, 1)");
  }

  @Test
  public void testBackgroundColorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("background-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      backgroundColor(
        zoom(
          exponential(
            stop(2, backgroundColor("rgba(0, 0, 0, 1)"))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getBackgroundColor());
    assertNotNull(layer.getBackgroundColor().getFunction());
    assertEquals(CameraFunction.class, layer.getBackgroundColor().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getBackgroundColor().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getBackgroundColor().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getBackgroundColor().getFunction().getStops()).size());
  }

  @Test
  public void testBackgroundColorAsIntConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("background-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(backgroundColor(Color.RED));
    assertEquals(layer.getBackgroundColorAsInt(), Color.RED);
  }

  @Test
  public void testBackgroundPatternAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("background-pattern");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(backgroundPattern("pedestrian-polygon"));
    assertEquals((String) layer.getBackgroundPattern().getValue(), (String) "pedestrian-polygon");
  }

  @Test
  public void testBackgroundPatternAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("background-pattern");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      backgroundPattern(
        zoom(
          interval(
            stop(2, backgroundPattern("pedestrian-polygon"))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getBackgroundPattern());
    assertNotNull(layer.getBackgroundPattern().getFunction());
    assertEquals(CameraFunction.class, layer.getBackgroundPattern().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getBackgroundPattern().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getBackgroundPattern().getFunction().getStops()).size());
  }

  @Test
  public void testBackgroundOpacityAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("background-opacity");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(backgroundOpacity(0.3f));
    assertEquals((Float) layer.getBackgroundOpacity().getValue(), (Float) 0.3f);
  }

  @Test
  public void testBackgroundOpacityAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("background-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      backgroundOpacity(
        zoom(
          exponential(
            stop(2, backgroundOpacity(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getBackgroundOpacity());
    assertNotNull(layer.getBackgroundOpacity().getFunction());
    assertEquals(CameraFunction.class, layer.getBackgroundOpacity().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getBackgroundOpacity().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getBackgroundOpacity().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getBackgroundOpacity().getFunction().getStops()).size());
  }


  @After
  public void unregisterIntentServiceIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }
}
