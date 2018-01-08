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
import com.mapbox.mapboxsdk.style.layers.BackgroundLayer;
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
 * Basic smoke tests for BackgroundLayer
 */
@RunWith(AndroidJUnit4.class)
public class BackgroundLayerTest extends BaseActivityTest {

  private BackgroundLayer layer;

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  private void setupLayer() {
    Timber.i("Retrieving layer");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        layer = mapboxMap.getLayerAs("background");
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
  public void testBackgroundColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-colorTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setBackgroundColorTransition(options);
        assertEquals(layer.getBackgroundColorTransition(), options);
      }
    });
  }

  @Test
  public void testBackgroundColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(backgroundColor("rgba(0, 0, 0, 1)"));
        assertEquals((String) layer.getBackgroundColor().getValue(), (String) "rgba(0, 0, 0, 1)");
      }
    });
  }

  @Test
  public void testBackgroundColorAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
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
    });
  }

  @Test
  public void testBackgroundColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(backgroundColor(Color.RED));
        assertEquals(layer.getBackgroundColorAsInt(), Color.RED);
      }
    });
  }

  @Test
  public void testBackgroundPatternTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-patternTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setBackgroundPatternTransition(options);
        assertEquals(layer.getBackgroundPatternTransition(), options);
      }
    });
  }

  @Test
  public void testBackgroundPatternAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-pattern");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(backgroundPattern("pedestrian-polygon"));
        assertEquals((String) layer.getBackgroundPattern().getValue(), (String) "pedestrian-polygon");
      }
    });
  }

  @Test
  public void testBackgroundPatternAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-pattern");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
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
    });
  }

  @Test
  public void testBackgroundOpacityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-opacityTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setBackgroundOpacityTransition(options);
        assertEquals(layer.getBackgroundOpacityTransition(), options);
      }
    });
  }

  @Test
  public void testBackgroundOpacityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(backgroundOpacity(0.3f));
        assertEquals((Float) layer.getBackgroundOpacity().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testBackgroundOpacityAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
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
    });
  }

}
