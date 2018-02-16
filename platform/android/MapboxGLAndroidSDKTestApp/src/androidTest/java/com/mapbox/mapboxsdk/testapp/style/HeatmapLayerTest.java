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
import com.mapbox.mapboxsdk.style.layers.HeatmapLayer;
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
 * Basic smoke tests for HeatmapLayer
 */
@RunWith(AndroidJUnit4.class)
public class HeatmapLayerTest extends BaseActivityTest {

  private HeatmapLayer layer;

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
          layer = new HeatmapLayer("my-layer", "composite");
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
  public void testSourceLayer() {
    validateTestSetup();
    setupLayer();
    Timber.i("SourceLayer");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Get initial
        assertEquals(layer.getSourceLayer(), "composite");

        // Set
        final String sourceLayer = "test";
        layer.setSourceLayer(sourceLayer);
        assertEquals(layer.getSourceLayer(), sourceLayer);
      }
    });
  }

  @Test
  public void testHeatmapRadiusTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-radiusTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setHeatmapRadiusTransition(options);
        assertEquals(layer.getHeatmapRadiusTransition(), options);
      }
    });
  }

  @Test
  public void testHeatmapRadiusAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-radius");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(heatmapRadius(0.3f));
        assertEquals((Float) layer.getHeatmapRadius().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testHeatmapRadiusAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-radius");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          heatmapRadius(
            zoom(
              exponential(
                stop(2, heatmapRadius(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getHeatmapRadius());
        assertNotNull(layer.getHeatmapRadius().getFunction());
        assertEquals(CameraFunction.class, layer.getHeatmapRadius().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getHeatmapRadius().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getHeatmapRadius().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getHeatmapRadius().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testHeatmapRadiusAsIdentitySourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-radius");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          heatmapRadius(property("FeaturePropertyA", Stops.<Float>identity()))
        );

        // Verify
        assertNotNull(layer.getHeatmapRadius());
        assertNotNull(layer.getHeatmapRadius().getFunction());
        assertEquals(SourceFunction.class, layer.getHeatmapRadius().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getHeatmapRadius().getFunction()).getProperty());
        assertEquals(IdentityStops.class, layer.getHeatmapRadius().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testHeatmapRadiusAsExponentialSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-radius");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          heatmapRadius(
            property(
              "FeaturePropertyA",
              exponential(
                stop(0.3f, heatmapRadius(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getHeatmapRadius());
        assertNotNull(layer.getHeatmapRadius().getFunction());
        assertEquals(SourceFunction.class, layer.getHeatmapRadius().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getHeatmapRadius().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getHeatmapRadius().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testHeatmapRadiusAsCategoricalSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-radius");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          heatmapRadius(
            property(
              "FeaturePropertyA",
              categorical(
                stop(1.0f, heatmapRadius(0.3f))
              )
            ).withDefaultValue(heatmapRadius(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getHeatmapRadius());
        assertNotNull(layer.getHeatmapRadius().getFunction());
        assertEquals(SourceFunction.class, layer.getHeatmapRadius().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getHeatmapRadius().getFunction()).getProperty());
        assertEquals(CategoricalStops.class, layer.getHeatmapRadius().getFunction().getStops().getClass());
        assertNotNull(((SourceFunction) layer.getHeatmapRadius().getFunction()).getDefaultValue());
        assertNotNull(((SourceFunction) layer.getHeatmapRadius().getFunction()).getDefaultValue().getValue());
        assertEquals(0.3f, ((SourceFunction) layer.getHeatmapRadius().getFunction()).getDefaultValue().getValue());
      }
    });

  }

  @Test
  public void testHeatmapRadiusAsCompositeFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-radius");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          heatmapRadius(
            composite(
              "FeaturePropertyA",
              exponential(
                stop(0, 0.3f, heatmapRadius(0.9f))
              ).withBase(0.5f)
            ).withDefaultValue(heatmapRadius(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getHeatmapRadius());
        assertNotNull(layer.getHeatmapRadius().getFunction());
        assertEquals(CompositeFunction.class, layer.getHeatmapRadius().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getHeatmapRadius().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getHeatmapRadius().getFunction().getStops().getClass());
        assertEquals(1, ((ExponentialStops) layer.getHeatmapRadius().getFunction().getStops()).size());

        ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
          (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getHeatmapRadius().getFunction().getStops();
        Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
        assertEquals(0f, stop.in.zoom, 0.001);
        assertEquals(0.3f, stop.in.value, 0.001f);
        assertEquals(0.9f, stop.out, 0.001f);
      }
    });
  }

  @Test
  public void testHeatmapWeightAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-weight");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(heatmapWeight(0.3f));
        assertEquals((Float) layer.getHeatmapWeight().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testHeatmapWeightAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-weight");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          heatmapWeight(
            zoom(
              exponential(
                stop(2, heatmapWeight(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getHeatmapWeight());
        assertNotNull(layer.getHeatmapWeight().getFunction());
        assertEquals(CameraFunction.class, layer.getHeatmapWeight().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getHeatmapWeight().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getHeatmapWeight().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getHeatmapWeight().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testHeatmapWeightAsIdentitySourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-weight");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          heatmapWeight(property("FeaturePropertyA", Stops.<Float>identity()))
        );

        // Verify
        assertNotNull(layer.getHeatmapWeight());
        assertNotNull(layer.getHeatmapWeight().getFunction());
        assertEquals(SourceFunction.class, layer.getHeatmapWeight().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getHeatmapWeight().getFunction()).getProperty());
        assertEquals(IdentityStops.class, layer.getHeatmapWeight().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testHeatmapWeightAsExponentialSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-weight");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          heatmapWeight(
            property(
              "FeaturePropertyA",
              exponential(
                stop(0.3f, heatmapWeight(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getHeatmapWeight());
        assertNotNull(layer.getHeatmapWeight().getFunction());
        assertEquals(SourceFunction.class, layer.getHeatmapWeight().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getHeatmapWeight().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getHeatmapWeight().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testHeatmapWeightAsCategoricalSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-weight");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          heatmapWeight(
            property(
              "FeaturePropertyA",
              categorical(
                stop(1.0f, heatmapWeight(0.3f))
              )
            ).withDefaultValue(heatmapWeight(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getHeatmapWeight());
        assertNotNull(layer.getHeatmapWeight().getFunction());
        assertEquals(SourceFunction.class, layer.getHeatmapWeight().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getHeatmapWeight().getFunction()).getProperty());
        assertEquals(CategoricalStops.class, layer.getHeatmapWeight().getFunction().getStops().getClass());
        assertNotNull(((SourceFunction) layer.getHeatmapWeight().getFunction()).getDefaultValue());
        assertNotNull(((SourceFunction) layer.getHeatmapWeight().getFunction()).getDefaultValue().getValue());
        assertEquals(0.3f, ((SourceFunction) layer.getHeatmapWeight().getFunction()).getDefaultValue().getValue());
      }
    });

  }

  @Test
  public void testHeatmapWeightAsCompositeFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-weight");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          heatmapWeight(
            composite(
              "FeaturePropertyA",
              exponential(
                stop(0, 0.3f, heatmapWeight(0.9f))
              ).withBase(0.5f)
            ).withDefaultValue(heatmapWeight(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getHeatmapWeight());
        assertNotNull(layer.getHeatmapWeight().getFunction());
        assertEquals(CompositeFunction.class, layer.getHeatmapWeight().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getHeatmapWeight().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getHeatmapWeight().getFunction().getStops().getClass());
        assertEquals(1, ((ExponentialStops) layer.getHeatmapWeight().getFunction().getStops()).size());

        ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
          (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getHeatmapWeight().getFunction().getStops();
        Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
        assertEquals(0f, stop.in.zoom, 0.001);
        assertEquals(0.3f, stop.in.value, 0.001f);
        assertEquals(0.9f, stop.out, 0.001f);
      }
    });
  }

  @Test
  public void testHeatmapIntensityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-intensityTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setHeatmapIntensityTransition(options);
        assertEquals(layer.getHeatmapIntensityTransition(), options);
      }
    });
  }

  @Test
  public void testHeatmapIntensityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-intensity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(heatmapIntensity(0.3f));
        assertEquals((Float) layer.getHeatmapIntensity().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testHeatmapIntensityAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-intensity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          heatmapIntensity(
            zoom(
              exponential(
                stop(2, heatmapIntensity(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getHeatmapIntensity());
        assertNotNull(layer.getHeatmapIntensity().getFunction());
        assertEquals(CameraFunction.class, layer.getHeatmapIntensity().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getHeatmapIntensity().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getHeatmapIntensity().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getHeatmapIntensity().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testHeatmapOpacityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-opacityTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setHeatmapOpacityTransition(options);
        assertEquals(layer.getHeatmapOpacityTransition(), options);
      }
    });
  }

  @Test
  public void testHeatmapOpacityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(heatmapOpacity(0.3f));
        assertEquals((Float) layer.getHeatmapOpacity().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testHeatmapOpacityAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          heatmapOpacity(
            zoom(
              exponential(
                stop(2, heatmapOpacity(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getHeatmapOpacity());
        assertNotNull(layer.getHeatmapOpacity().getFunction());
        assertEquals(CameraFunction.class, layer.getHeatmapOpacity().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getHeatmapOpacity().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getHeatmapOpacity().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getHeatmapOpacity().getFunction().getStops()).size());
      }
    });
  }

}
