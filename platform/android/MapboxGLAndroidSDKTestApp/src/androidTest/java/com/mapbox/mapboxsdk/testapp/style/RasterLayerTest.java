// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.support.test.runner.AndroidJUnit4;

import timber.log.Timber;

import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.RasterLayer;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;

import org.junit.Test;
import org.junit.runner.RunWith;

import static com.mapbox.mapboxsdk.style.expressions.Expression.*;
import static com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke;
import static org.junit.Assert.*;
import static com.mapbox.mapboxsdk.style.layers.Property.*;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.*;

import com.mapbox.mapboxsdk.style.layers.TransitionOptions;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;

/**
 * Basic smoke tests for RasterLayer
 */
@RunWith(AndroidJUnit4.class)
public class RasterLayerTest extends BaseActivityTest {

  private RasterLayer layer;

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  private void setupLayer() {
    Timber.i("Retrieving layer");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      if ((layer = mapboxMap.getStyle().getLayerAs("my-layer")) == null) {
        Timber.i("Adding layer");
        layer = new RasterLayer("my-layer", "composite");
        layer.setSourceLayer("composite");
        mapboxMap.getStyle().addLayer(layer);
        // Layer reference is now stale, get new reference
        layer = mapboxMap.getStyle().getLayerAs("my-layer");
      }
    });
  }

  @Test
  public void testSourceId() {
    validateTestSetup();
    setupLayer();
    Timber.i("SourceId");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      // Get source id
      assertEquals(layer.getSourceId(), "composite");
    });
  }

  @Test
  public void testSetVisibility() {
    validateTestSetup();
    setupLayer();
    Timber.i("Visibility");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Get initial
      assertEquals(layer.getVisibility().getValue(), VISIBLE);

      // Set
      layer.setProperties(visibility(NONE));
      assertEquals(layer.getVisibility().getValue(), NONE);
    });
  }

  @Test
  public void testRasterOpacityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-opacityTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setRasterOpacityTransition(options);
      assertEquals(layer.getRasterOpacityTransition(), options);
    });
  }

  @Test
  public void testRasterOpacityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-opacity");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getRasterOpacity().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(rasterOpacity(propertyValue));
      assertEquals(layer.getRasterOpacity().getValue(), propertyValue);
    });
  }

  @Test
  public void testRasterHueRotateTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-hue-rotateTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setRasterHueRotateTransition(options);
      assertEquals(layer.getRasterHueRotateTransition(), options);
    });
  }

  @Test
  public void testRasterHueRotateAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-hue-rotate");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getRasterHueRotate().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(rasterHueRotate(propertyValue));
      assertEquals(layer.getRasterHueRotate().getValue(), propertyValue);
    });
  }

  @Test
  public void testRasterBrightnessMinTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-brightness-minTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setRasterBrightnessMinTransition(options);
      assertEquals(layer.getRasterBrightnessMinTransition(), options);
    });
  }

  @Test
  public void testRasterBrightnessMinAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-brightness-min");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getRasterBrightnessMin().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(rasterBrightnessMin(propertyValue));
      assertEquals(layer.getRasterBrightnessMin().getValue(), propertyValue);
    });
  }

  @Test
  public void testRasterBrightnessMaxTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-brightness-maxTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setRasterBrightnessMaxTransition(options);
      assertEquals(layer.getRasterBrightnessMaxTransition(), options);
    });
  }

  @Test
  public void testRasterBrightnessMaxAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-brightness-max");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getRasterBrightnessMax().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(rasterBrightnessMax(propertyValue));
      assertEquals(layer.getRasterBrightnessMax().getValue(), propertyValue);
    });
  }

  @Test
  public void testRasterSaturationTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-saturationTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setRasterSaturationTransition(options);
      assertEquals(layer.getRasterSaturationTransition(), options);
    });
  }

  @Test
  public void testRasterSaturationAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-saturation");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getRasterSaturation().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(rasterSaturation(propertyValue));
      assertEquals(layer.getRasterSaturation().getValue(), propertyValue);
    });
  }

  @Test
  public void testRasterContrastTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-contrastTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setRasterContrastTransition(options);
      assertEquals(layer.getRasterContrastTransition(), options);
    });
  }

  @Test
  public void testRasterContrastAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-contrast");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getRasterContrast().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(rasterContrast(propertyValue));
      assertEquals(layer.getRasterContrast().getValue(), propertyValue);
    });
  }

  @Test
  public void testRasterResamplingAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-resampling");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getRasterResampling().getValue());

      // Set and Get
      String propertyValue = RASTER_RESAMPLING_LINEAR;
      layer.setProperties(rasterResampling(propertyValue));
      assertEquals(layer.getRasterResampling().getValue(), propertyValue);
    });
  }

  @Test
  public void testRasterFadeDurationAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-fade-duration");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getRasterFadeDuration().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(rasterFadeDuration(propertyValue));
      assertEquals(layer.getRasterFadeDuration().getValue(), propertyValue);
    });
  }
}
