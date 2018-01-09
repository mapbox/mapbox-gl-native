// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.support.test.espresso.UiController;
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
import com.mapbox.mapboxsdk.style.layers.RasterLayer;
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;

import org.junit.Test;
import org.junit.runner.RunWith;

import static com.mapbox.mapboxsdk.style.functions.Function.*;
import static com.mapbox.mapboxsdk.style.functions.stops.Stop.stop;
import static com.mapbox.mapboxsdk.style.functions.stops.Stops.*;
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
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
          Timber.i("Adding layer");
          layer = new RasterLayer("my-layer", "composite");
          layer.setSourceLayer("composite");
          mapboxMap.addLayer(layer);
          // Layer reference is now stale, get new reference
          layer = mapboxMap.getLayerAs("my-layer");
        }
      }
    });
  }

  @Test
  public void testSetVisibility() {
    validateTestSetup();
    setupLayer();
    Timber.i("Visibility");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Get initial
        assertEquals(layer.getVisibility().getValue(), VISIBLE);

        // Set
        layer.setProperties(visibility(NONE));
        assertEquals(layer.getVisibility().getValue(), NONE);
      }
    });
  }

  @Test
  public void testRasterOpacityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-opacityTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setRasterOpacityTransition(options);
        assertEquals(layer.getRasterOpacityTransition(), options);
      }
    });
  }

  @Test
  public void testRasterOpacityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(rasterOpacity(0.3f));
        assertEquals((Float) layer.getRasterOpacity().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testRasterOpacityAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
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
    });
  }

  @Test
  public void testRasterHueRotateTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-hue-rotateTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setRasterHueRotateTransition(options);
        assertEquals(layer.getRasterHueRotateTransition(), options);
      }
    });
  }

  @Test
  public void testRasterHueRotateAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-hue-rotate");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(rasterHueRotate(0.3f));
        assertEquals((Float) layer.getRasterHueRotate().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testRasterHueRotateAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-hue-rotate");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
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
    });
  }

  @Test
  public void testRasterBrightnessMinTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-brightness-minTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setRasterBrightnessMinTransition(options);
        assertEquals(layer.getRasterBrightnessMinTransition(), options);
      }
    });
  }

  @Test
  public void testRasterBrightnessMinAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-brightness-min");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(rasterBrightnessMin(0.3f));
        assertEquals((Float) layer.getRasterBrightnessMin().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testRasterBrightnessMinAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-brightness-min");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
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
    });
  }

  @Test
  public void testRasterBrightnessMaxTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-brightness-maxTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setRasterBrightnessMaxTransition(options);
        assertEquals(layer.getRasterBrightnessMaxTransition(), options);
      }
    });
  }

  @Test
  public void testRasterBrightnessMaxAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-brightness-max");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(rasterBrightnessMax(0.3f));
        assertEquals((Float) layer.getRasterBrightnessMax().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testRasterBrightnessMaxAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-brightness-max");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
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
    });
  }

  @Test
  public void testRasterSaturationTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-saturationTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setRasterSaturationTransition(options);
        assertEquals(layer.getRasterSaturationTransition(), options);
      }
    });
  }

  @Test
  public void testRasterSaturationAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-saturation");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(rasterSaturation(0.3f));
        assertEquals((Float) layer.getRasterSaturation().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testRasterSaturationAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-saturation");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
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
    });
  }

  @Test
  public void testRasterContrastTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-contrastTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setRasterContrastTransition(options);
        assertEquals(layer.getRasterContrastTransition(), options);
      }
    });
  }

  @Test
  public void testRasterContrastAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-contrast");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(rasterContrast(0.3f));
        assertEquals((Float) layer.getRasterContrast().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testRasterContrastAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-contrast");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
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
    });
  }

  @Test
  public void testRasterFadeDurationAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-fade-duration");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(rasterFadeDuration(0.3f));
        assertEquals((Float) layer.getRasterFadeDuration().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testRasterFadeDurationAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("raster-fade-duration");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
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
    });
  }

}
