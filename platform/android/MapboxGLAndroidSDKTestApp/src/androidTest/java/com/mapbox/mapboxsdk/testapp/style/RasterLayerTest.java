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
      if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
        Timber.i("Adding layer");
        layer = new RasterLayer("my-layer", "composite");
        layer.setSourceLayer("composite");
        mapboxMap.addLayer(layer);
        // Layer reference is now stale, get new reference
        layer = mapboxMap.getLayerAs("my-layer");
      }
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

      // Set and Get
      layer.setProperties(rasterOpacity(0.3f));
      assertEquals((Float) layer.getRasterOpacity().getValue(), (Float) 0.3f);
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

      // Set and Get
      layer.setProperties(rasterHueRotate(0.3f));
      assertEquals((Float) layer.getRasterHueRotate().getValue(), (Float) 0.3f);
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

      // Set and Get
      layer.setProperties(rasterBrightnessMin(0.3f));
      assertEquals((Float) layer.getRasterBrightnessMin().getValue(), (Float) 0.3f);
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

      // Set and Get
      layer.setProperties(rasterBrightnessMax(0.3f));
      assertEquals((Float) layer.getRasterBrightnessMax().getValue(), (Float) 0.3f);
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

      // Set and Get
      layer.setProperties(rasterSaturation(0.3f));
      assertEquals((Float) layer.getRasterSaturation().getValue(), (Float) 0.3f);
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

      // Set and Get
      layer.setProperties(rasterContrast(0.3f));
      assertEquals((Float) layer.getRasterContrast().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testRasterFadeDurationAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-fade-duration");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(rasterFadeDuration(0.3f));
      assertEquals((Float) layer.getRasterFadeDuration().getValue(), (Float) 0.3f);
    });
  }
}