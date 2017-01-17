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
import com.mapbox.mapboxsdk.style.layers.RasterLayer;
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
 * Basic smoke tests for RasterLayer
 */
@RunWith(AndroidJUnit4.class)
public class RasterLayerTest extends BaseStyleTest {

  @Rule
  public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

  private RasterLayer layer;

  private OnMapReadyIdlingResource idlingResource;

  private MapboxMap mapboxMap;

  @Before
  public void setup() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
    mapboxMap = rule.getActivity().getMapboxMap();

    if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
      Timber.i("Adding layer");
      layer = new RasterLayer("my-layer", "composite");
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
  public void testRasterOpacityAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("raster-opacity");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(rasterOpacity(0.3f));
    assertEquals((Float) layer.getRasterOpacity().getValue(), (Float) 0.3f);
  }

  @Test
  public void testRasterOpacityAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("raster-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      rasterOpacity(
        zoom(
          exponential(
            stop(2, rasterOpacity(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getRasterOpacity());
    assertNotNull(layer.getRasterOpacity().getFunction());
    assertEquals(CameraFunction.class, layer.getRasterOpacity().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getRasterOpacity().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getRasterOpacity().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getRasterOpacity().getFunction().getStops()).size());
  }

  @Test
  public void testRasterHueRotateAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("raster-hue-rotate");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(rasterHueRotate(0.3f));
    assertEquals((Float) layer.getRasterHueRotate().getValue(), (Float) 0.3f);
  }

  @Test
  public void testRasterHueRotateAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("raster-hue-rotate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      rasterHueRotate(
        zoom(
          exponential(
            stop(2, rasterHueRotate(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getRasterHueRotate());
    assertNotNull(layer.getRasterHueRotate().getFunction());
    assertEquals(CameraFunction.class, layer.getRasterHueRotate().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getRasterHueRotate().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getRasterHueRotate().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getRasterHueRotate().getFunction().getStops()).size());
  }

  @Test
  public void testRasterBrightnessMinAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("raster-brightness-min");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(rasterBrightnessMin(0.3f));
    assertEquals((Float) layer.getRasterBrightnessMin().getValue(), (Float) 0.3f);
  }

  @Test
  public void testRasterBrightnessMinAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("raster-brightness-min");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      rasterBrightnessMin(
        zoom(
          exponential(
            stop(2, rasterBrightnessMin(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getRasterBrightnessMin());
    assertNotNull(layer.getRasterBrightnessMin().getFunction());
    assertEquals(CameraFunction.class, layer.getRasterBrightnessMin().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getRasterBrightnessMin().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getRasterBrightnessMin().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getRasterBrightnessMin().getFunction().getStops()).size());
  }

  @Test
  public void testRasterBrightnessMaxAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("raster-brightness-max");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(rasterBrightnessMax(0.3f));
    assertEquals((Float) layer.getRasterBrightnessMax().getValue(), (Float) 0.3f);
  }

  @Test
  public void testRasterBrightnessMaxAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("raster-brightness-max");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      rasterBrightnessMax(
        zoom(
          exponential(
            stop(2, rasterBrightnessMax(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getRasterBrightnessMax());
    assertNotNull(layer.getRasterBrightnessMax().getFunction());
    assertEquals(CameraFunction.class, layer.getRasterBrightnessMax().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getRasterBrightnessMax().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getRasterBrightnessMax().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getRasterBrightnessMax().getFunction().getStops()).size());
  }

  @Test
  public void testRasterSaturationAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("raster-saturation");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(rasterSaturation(0.3f));
    assertEquals((Float) layer.getRasterSaturation().getValue(), (Float) 0.3f);
  }

  @Test
  public void testRasterSaturationAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("raster-saturation");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      rasterSaturation(
        zoom(
          exponential(
            stop(2, rasterSaturation(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getRasterSaturation());
    assertNotNull(layer.getRasterSaturation().getFunction());
    assertEquals(CameraFunction.class, layer.getRasterSaturation().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getRasterSaturation().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getRasterSaturation().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getRasterSaturation().getFunction().getStops()).size());
  }

  @Test
  public void testRasterContrastAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("raster-contrast");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(rasterContrast(0.3f));
    assertEquals((Float) layer.getRasterContrast().getValue(), (Float) 0.3f);
  }

  @Test
  public void testRasterContrastAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("raster-contrast");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      rasterContrast(
        zoom(
          exponential(
            stop(2, rasterContrast(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getRasterContrast());
    assertNotNull(layer.getRasterContrast().getFunction());
    assertEquals(CameraFunction.class, layer.getRasterContrast().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getRasterContrast().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getRasterContrast().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getRasterContrast().getFunction().getStops()).size());
  }

  @Test
  public void testRasterFadeDurationAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("raster-fade-duration");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(rasterFadeDuration(0.3f));
    assertEquals((Float) layer.getRasterFadeDuration().getValue(), (Float) 0.3f);
  }

  @Test
  public void testRasterFadeDurationAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("raster-fade-duration");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      rasterFadeDuration(
        zoom(
          exponential(
            stop(2, rasterFadeDuration(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getRasterFadeDuration());
    assertNotNull(layer.getRasterFadeDuration().getFunction());
    assertEquals(CameraFunction.class, layer.getRasterFadeDuration().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getRasterFadeDuration().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getRasterFadeDuration().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getRasterFadeDuration().getFunction().getStops()).size());
  }


  @After
  public void unregisterIntentServiceIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }
}
