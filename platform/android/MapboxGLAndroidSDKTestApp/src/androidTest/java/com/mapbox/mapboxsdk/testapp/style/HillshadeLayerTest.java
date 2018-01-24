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
import com.mapbox.mapboxsdk.style.layers.HillshadeLayer;
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
 * Basic smoke tests for HillshadeLayer
 */
@RunWith(AndroidJUnit4.class)
public class HillshadeLayerTest extends BaseActivityTest {

  private HillshadeLayer layer;

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
          layer = new HillshadeLayer("my-layer", "composite");
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
  public void testHillshadeIlluminationDirectionAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-illumination-direction");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(hillshadeIlluminationDirection(0.3f));
        assertEquals((Float) layer.getHillshadeIlluminationDirection().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testHillshadeIlluminationDirectionAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-illumination-direction");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          hillshadeIlluminationDirection(
            zoom(
              exponential(
                stop(2, hillshadeIlluminationDirection(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getHillshadeIlluminationDirection());
        assertNotNull(layer.getHillshadeIlluminationDirection().getFunction());
        assertEquals(CameraFunction.class, layer.getHillshadeIlluminationDirection().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getHillshadeIlluminationDirection().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getHillshadeIlluminationDirection().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getHillshadeIlluminationDirection().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testHillshadeIlluminationAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-illumination-anchor");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(hillshadeIlluminationAnchor(HILLSHADE_ILLUMINATION_ANCHOR_MAP));
        assertEquals((String) layer.getHillshadeIlluminationAnchor().getValue(), (String) HILLSHADE_ILLUMINATION_ANCHOR_MAP);
      }
    });
  }

  @Test
  public void testHillshadeIlluminationAnchorAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-illumination-anchor");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          hillshadeIlluminationAnchor(
            zoom(
              interval(
                stop(2, hillshadeIlluminationAnchor(HILLSHADE_ILLUMINATION_ANCHOR_MAP))
              )
            )
          )
        );

        // Verify
        assertNotNull(layer.getHillshadeIlluminationAnchor());
        assertNotNull(layer.getHillshadeIlluminationAnchor().getFunction());
        assertEquals(CameraFunction.class, layer.getHillshadeIlluminationAnchor().getFunction().getClass());
        assertEquals(IntervalStops.class, layer.getHillshadeIlluminationAnchor().getFunction().getStops().getClass());
        assertEquals(1, ((IntervalStops) layer.getHillshadeIlluminationAnchor().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testHillshadeExaggerationTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-exaggerationTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setHillshadeExaggerationTransition(options);
        assertEquals(layer.getHillshadeExaggerationTransition(), options);
      }
    });
  }

  @Test
  public void testHillshadeExaggerationAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-exaggeration");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(hillshadeExaggeration(0.3f));
        assertEquals((Float) layer.getHillshadeExaggeration().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testHillshadeExaggerationAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-exaggeration");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          hillshadeExaggeration(
            zoom(
              exponential(
                stop(2, hillshadeExaggeration(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getHillshadeExaggeration());
        assertNotNull(layer.getHillshadeExaggeration().getFunction());
        assertEquals(CameraFunction.class, layer.getHillshadeExaggeration().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getHillshadeExaggeration().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getHillshadeExaggeration().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getHillshadeExaggeration().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testHillshadeShadowColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-shadow-colorTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setHillshadeShadowColorTransition(options);
        assertEquals(layer.getHillshadeShadowColorTransition(), options);
      }
    });
  }

  @Test
  public void testHillshadeShadowColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-shadow-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(hillshadeShadowColor("rgba(0, 0, 0, 1)"));
        assertEquals((String) layer.getHillshadeShadowColor().getValue(), (String) "rgba(0, 0, 0, 1)");
      }
    });
  }

  @Test
  public void testHillshadeShadowColorAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-shadow-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          hillshadeShadowColor(
            zoom(
              exponential(
                stop(2, hillshadeShadowColor("rgba(0, 0, 0, 1)"))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getHillshadeShadowColor());
        assertNotNull(layer.getHillshadeShadowColor().getFunction());
        assertEquals(CameraFunction.class, layer.getHillshadeShadowColor().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getHillshadeShadowColor().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getHillshadeShadowColor().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getHillshadeShadowColor().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testHillshadeShadowColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-shadow-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(hillshadeShadowColor(Color.RED));
        assertEquals(layer.getHillshadeShadowColorAsInt(), Color.RED);
      }
    });
  }

  @Test
  public void testHillshadeHighlightColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-highlight-colorTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setHillshadeHighlightColorTransition(options);
        assertEquals(layer.getHillshadeHighlightColorTransition(), options);
      }
    });
  }

  @Test
  public void testHillshadeHighlightColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-highlight-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(hillshadeHighlightColor("rgba(0, 0, 0, 1)"));
        assertEquals((String) layer.getHillshadeHighlightColor().getValue(), (String) "rgba(0, 0, 0, 1)");
      }
    });
  }

  @Test
  public void testHillshadeHighlightColorAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-highlight-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          hillshadeHighlightColor(
            zoom(
              exponential(
                stop(2, hillshadeHighlightColor("rgba(0, 0, 0, 1)"))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getHillshadeHighlightColor());
        assertNotNull(layer.getHillshadeHighlightColor().getFunction());
        assertEquals(CameraFunction.class, layer.getHillshadeHighlightColor().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getHillshadeHighlightColor().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getHillshadeHighlightColor().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getHillshadeHighlightColor().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testHillshadeHighlightColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-highlight-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(hillshadeHighlightColor(Color.RED));
        assertEquals(layer.getHillshadeHighlightColorAsInt(), Color.RED);
      }
    });
  }

  @Test
  public void testHillshadeAccentColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-accent-colorTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setHillshadeAccentColorTransition(options);
        assertEquals(layer.getHillshadeAccentColorTransition(), options);
      }
    });
  }

  @Test
  public void testHillshadeAccentColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-accent-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(hillshadeAccentColor("rgba(0, 0, 0, 1)"));
        assertEquals((String) layer.getHillshadeAccentColor().getValue(), (String) "rgba(0, 0, 0, 1)");
      }
    });
  }

  @Test
  public void testHillshadeAccentColorAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-accent-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          hillshadeAccentColor(
            zoom(
              exponential(
                stop(2, hillshadeAccentColor("rgba(0, 0, 0, 1)"))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getHillshadeAccentColor());
        assertNotNull(layer.getHillshadeAccentColor().getFunction());
        assertEquals(CameraFunction.class, layer.getHillshadeAccentColor().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getHillshadeAccentColor().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getHillshadeAccentColor().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getHillshadeAccentColor().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testHillshadeAccentColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-accent-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(hillshadeAccentColor(Color.RED));
        assertEquals(layer.getHillshadeAccentColorAsInt(), Color.RED);
      }
    });
  }

}
