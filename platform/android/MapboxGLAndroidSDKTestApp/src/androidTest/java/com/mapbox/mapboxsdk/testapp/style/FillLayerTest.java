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
import com.mapbox.mapboxsdk.style.layers.FillLayer;
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
 * Basic smoke tests for FillLayer
 */
@RunWith(AndroidJUnit4.class)
public class FillLayerTest extends BaseActivityTest {

  private FillLayer layer;

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
          layer = new FillLayer("my-layer", "composite");
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
  public void testFillAntialiasAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-antialias");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(fillAntialias(true));
        assertEquals((Boolean) layer.getFillAntialias().getValue(), (Boolean) true);
      }
    });
  }

  @Test
  public void testFillAntialiasAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-antialias");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillAntialias(
            zoom(
              interval(
                stop(2, fillAntialias(true))
              )
            )
          )
        );

        // Verify
        assertNotNull(layer.getFillAntialias());
        assertNotNull(layer.getFillAntialias().getFunction());
        assertEquals(CameraFunction.class, layer.getFillAntialias().getFunction().getClass());
        assertEquals(IntervalStops.class, layer.getFillAntialias().getFunction().getStops().getClass());
        assertEquals(1, ((IntervalStops) layer.getFillAntialias().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testFillOpacityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-opacityTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setFillOpacityTransition(options);
        assertEquals(layer.getFillOpacityTransition(), options);
      }
    });
  }

  @Test
  public void testFillOpacityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(fillOpacity(0.3f));
        assertEquals((Float) layer.getFillOpacity().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testFillOpacityAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillOpacity(
            zoom(
              exponential(
                stop(2, fillOpacity(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getFillOpacity());
        assertNotNull(layer.getFillOpacity().getFunction());
        assertEquals(CameraFunction.class, layer.getFillOpacity().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getFillOpacity().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getFillOpacity().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getFillOpacity().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testFillOpacityAsIdentitySourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillOpacity(property("FeaturePropertyA", Stops.<Float>identity()))
        );

        // Verify
        assertNotNull(layer.getFillOpacity());
        assertNotNull(layer.getFillOpacity().getFunction());
        assertEquals(SourceFunction.class, layer.getFillOpacity().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillOpacity().getFunction()).getProperty());
        assertEquals(IdentityStops.class, layer.getFillOpacity().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testFillOpacityAsExponentialSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillOpacity(
            property(
              "FeaturePropertyA",
              exponential(
                stop(0.3f, fillOpacity(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getFillOpacity());
        assertNotNull(layer.getFillOpacity().getFunction());
        assertEquals(SourceFunction.class, layer.getFillOpacity().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillOpacity().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getFillOpacity().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testFillOpacityAsCategoricalSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillOpacity(
            property(
              "FeaturePropertyA",
              categorical(
                stop(1.0f, fillOpacity(0.3f))
              )
            ).withDefaultValue(fillOpacity(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getFillOpacity());
        assertNotNull(layer.getFillOpacity().getFunction());
        assertEquals(SourceFunction.class, layer.getFillOpacity().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillOpacity().getFunction()).getProperty());
        assertEquals(CategoricalStops.class, layer.getFillOpacity().getFunction().getStops().getClass());
        assertNotNull(((SourceFunction) layer.getFillOpacity().getFunction()).getDefaultValue());
        assertNotNull(((SourceFunction) layer.getFillOpacity().getFunction()).getDefaultValue().getValue());
        assertEquals(0.3f, ((SourceFunction) layer.getFillOpacity().getFunction()).getDefaultValue().getValue());
      }
    });

  }

  @Test
  public void testFillOpacityAsCompositeFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillOpacity(
            composite(
              "FeaturePropertyA",
              exponential(
                stop(0, 0.3f, fillOpacity(0.9f))
              ).withBase(0.5f)
            ).withDefaultValue(fillOpacity(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getFillOpacity());
        assertNotNull(layer.getFillOpacity().getFunction());
        assertEquals(CompositeFunction.class, layer.getFillOpacity().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getFillOpacity().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getFillOpacity().getFunction().getStops().getClass());
        assertEquals(1, ((ExponentialStops) layer.getFillOpacity().getFunction().getStops()).size());

        ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
          (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getFillOpacity().getFunction().getStops();
        Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
        assertEquals(0f, stop.in.zoom, 0.001);
        assertEquals(0.3f, stop.in.value, 0.001f);
        assertEquals(0.9f, stop.out, 0.001f);
      }
    });
  }

  @Test
  public void testFillColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-colorTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setFillColorTransition(options);
        assertEquals(layer.getFillColorTransition(), options);
      }
    });
  }

  @Test
  public void testFillColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(fillColor("rgba(0, 0, 0, 1)"));
        assertEquals((String) layer.getFillColor().getValue(), (String) "rgba(0, 0, 0, 1)");
      }
    });
  }

  @Test
  public void testFillColorAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillColor(
            zoom(
              exponential(
                stop(2, fillColor("rgba(0, 0, 0, 1)"))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getFillColor());
        assertNotNull(layer.getFillColor().getFunction());
        assertEquals(CameraFunction.class, layer.getFillColor().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getFillColor().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getFillColor().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getFillColor().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testFillColorAsIdentitySourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillColor(property("FeaturePropertyA", Stops.<String>identity()))
        );

        // Verify
        assertNotNull(layer.getFillColor());
        assertNotNull(layer.getFillColor().getFunction());
        assertEquals(SourceFunction.class, layer.getFillColor().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillColor().getFunction()).getProperty());
        assertEquals(IdentityStops.class, layer.getFillColor().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testFillColorAsExponentialSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillColor(
            property(
              "FeaturePropertyA",
              exponential(
                stop(Color.RED, fillColor(Color.RED))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getFillColor());
        assertNotNull(layer.getFillColor().getFunction());
        assertEquals(SourceFunction.class, layer.getFillColor().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillColor().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getFillColor().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testFillColorAsCategoricalSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillColor(
            property(
              "FeaturePropertyA",
              categorical(
                stop("valueA", fillColor(Color.RED))
              )
            ).withDefaultValue(fillColor(Color.GREEN))
          )
        );

        // Verify
        assertNotNull(layer.getFillColor());
        assertNotNull(layer.getFillColor().getFunction());
        assertEquals(SourceFunction.class, layer.getFillColor().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillColor().getFunction()).getProperty());
        assertEquals(CategoricalStops.class, layer.getFillColor().getFunction().getStops().getClass());
        assertNotNull(((SourceFunction) layer.getFillColor().getFunction()).getDefaultValue());
        assertNotNull(((SourceFunction) layer.getFillColor().getFunction()).getDefaultValue().getValue());
        assertEquals(Color.GREEN, (int) ((SourceFunction) layer.getFillColor().getFunction()).getDefaultValue().getColorInt());
      }
    });

  }

  @Test
  public void testFillColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(fillColor(Color.RED));
        assertEquals(layer.getFillColorAsInt(), Color.RED);
      }
    });
  }

  @Test
  public void testFillOutlineColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-outline-colorTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setFillOutlineColorTransition(options);
        assertEquals(layer.getFillOutlineColorTransition(), options);
      }
    });
  }

  @Test
  public void testFillOutlineColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-outline-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(fillOutlineColor("rgba(0, 0, 0, 1)"));
        assertEquals((String) layer.getFillOutlineColor().getValue(), (String) "rgba(0, 0, 0, 1)");
      }
    });
  }

  @Test
  public void testFillOutlineColorAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-outline-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillOutlineColor(
            zoom(
              exponential(
                stop(2, fillOutlineColor("rgba(0, 0, 0, 1)"))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getFillOutlineColor());
        assertNotNull(layer.getFillOutlineColor().getFunction());
        assertEquals(CameraFunction.class, layer.getFillOutlineColor().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getFillOutlineColor().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getFillOutlineColor().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getFillOutlineColor().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testFillOutlineColorAsIdentitySourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-outline-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillOutlineColor(property("FeaturePropertyA", Stops.<String>identity()))
        );

        // Verify
        assertNotNull(layer.getFillOutlineColor());
        assertNotNull(layer.getFillOutlineColor().getFunction());
        assertEquals(SourceFunction.class, layer.getFillOutlineColor().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillOutlineColor().getFunction()).getProperty());
        assertEquals(IdentityStops.class, layer.getFillOutlineColor().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testFillOutlineColorAsExponentialSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-outline-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillOutlineColor(
            property(
              "FeaturePropertyA",
              exponential(
                stop(Color.RED, fillOutlineColor(Color.RED))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getFillOutlineColor());
        assertNotNull(layer.getFillOutlineColor().getFunction());
        assertEquals(SourceFunction.class, layer.getFillOutlineColor().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillOutlineColor().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getFillOutlineColor().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testFillOutlineColorAsCategoricalSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-outline-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillOutlineColor(
            property(
              "FeaturePropertyA",
              categorical(
                stop("valueA", fillOutlineColor(Color.RED))
              )
            ).withDefaultValue(fillOutlineColor(Color.GREEN))
          )
        );

        // Verify
        assertNotNull(layer.getFillOutlineColor());
        assertNotNull(layer.getFillOutlineColor().getFunction());
        assertEquals(SourceFunction.class, layer.getFillOutlineColor().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getFillOutlineColor().getFunction()).getProperty());
        assertEquals(CategoricalStops.class, layer.getFillOutlineColor().getFunction().getStops().getClass());
        assertNotNull(((SourceFunction) layer.getFillOutlineColor().getFunction()).getDefaultValue());
        assertNotNull(((SourceFunction) layer.getFillOutlineColor().getFunction()).getDefaultValue().getValue());
        assertEquals(Color.GREEN, (int) ((SourceFunction) layer.getFillOutlineColor().getFunction()).getDefaultValue().getColorInt());
      }
    });

  }

  @Test
  public void testFillOutlineColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-outline-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(fillOutlineColor(Color.RED));
        assertEquals(layer.getFillOutlineColorAsInt(), Color.RED);
      }
    });
  }

  @Test
  public void testFillTranslateTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-translateTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setFillTranslateTransition(options);
        assertEquals(layer.getFillTranslateTransition(), options);
      }
    });
  }

  @Test
  public void testFillTranslateAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-translate");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(fillTranslate(new Float[] {0f, 0f}));
        assertEquals((Float[]) layer.getFillTranslate().getValue(), (Float[]) new Float[] {0f, 0f});
      }
    });
  }

  @Test
  public void testFillTranslateAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-translate");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillTranslate(
            zoom(
              exponential(
                stop(2, fillTranslate(new Float[] {0f, 0f}))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getFillTranslate());
        assertNotNull(layer.getFillTranslate().getFunction());
        assertEquals(CameraFunction.class, layer.getFillTranslate().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getFillTranslate().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getFillTranslate().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getFillTranslate().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testFillTranslateAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-translate-anchor");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(fillTranslateAnchor(FILL_TRANSLATE_ANCHOR_MAP));
        assertEquals((String) layer.getFillTranslateAnchor().getValue(), (String) FILL_TRANSLATE_ANCHOR_MAP);
      }
    });
  }

  @Test
  public void testFillTranslateAnchorAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-translate-anchor");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillTranslateAnchor(
            zoom(
              interval(
                stop(2, fillTranslateAnchor(FILL_TRANSLATE_ANCHOR_MAP))
              )
            )
          )
        );

        // Verify
        assertNotNull(layer.getFillTranslateAnchor());
        assertNotNull(layer.getFillTranslateAnchor().getFunction());
        assertEquals(CameraFunction.class, layer.getFillTranslateAnchor().getFunction().getClass());
        assertEquals(IntervalStops.class, layer.getFillTranslateAnchor().getFunction().getStops().getClass());
        assertEquals(1, ((IntervalStops) layer.getFillTranslateAnchor().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testFillPatternTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-patternTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setFillPatternTransition(options);
        assertEquals(layer.getFillPatternTransition(), options);
      }
    });
  }

  @Test
  public void testFillPatternAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-pattern");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(fillPattern("pedestrian-polygon"));
        assertEquals((String) layer.getFillPattern().getValue(), (String) "pedestrian-polygon");
      }
    });
  }

  @Test
  public void testFillPatternAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-pattern");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          fillPattern(
            zoom(
              interval(
                stop(2, fillPattern("pedestrian-polygon"))
              )
            )
          )
        );

        // Verify
        assertNotNull(layer.getFillPattern());
        assertNotNull(layer.getFillPattern().getFunction());
        assertEquals(CameraFunction.class, layer.getFillPattern().getFunction().getClass());
        assertEquals(IntervalStops.class, layer.getFillPattern().getFunction().getStops().getClass());
        assertEquals(1, ((IntervalStops) layer.getFillPattern().getFunction().getStops()).size());
      }
    });
  }

}
