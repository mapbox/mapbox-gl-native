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
import com.mapbox.mapboxsdk.style.layers.LineLayer;
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
 * Basic smoke tests for LineLayer
 */
@RunWith(AndroidJUnit4.class)
public class LineLayerTest extends BaseActivityTest {

  private LineLayer layer;

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
          layer = new LineLayer("my-layer", "composite");
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
  public void testLineCapAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-cap");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(lineCap(LINE_CAP_BUTT));
        assertEquals((String) layer.getLineCap().getValue(), (String) LINE_CAP_BUTT);
      }
    });
  }

  @Test
  public void testLineCapAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-cap");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineCap(
            zoom(
              interval(
                stop(2, lineCap(LINE_CAP_BUTT))
              )
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineCap());
        assertNotNull(layer.getLineCap().getFunction());
        assertEquals(CameraFunction.class, layer.getLineCap().getFunction().getClass());
        assertEquals(IntervalStops.class, layer.getLineCap().getFunction().getStops().getClass());
        assertEquals(1, ((IntervalStops) layer.getLineCap().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testLineJoinAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-join");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(lineJoin(LINE_JOIN_BEVEL));
        assertEquals((String) layer.getLineJoin().getValue(), (String) LINE_JOIN_BEVEL);
      }
    });
  }

  @Test
  public void testLineJoinAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-join");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineJoin(
            zoom(
              interval(
                stop(2, lineJoin(LINE_JOIN_BEVEL))
              )
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineJoin());
        assertNotNull(layer.getLineJoin().getFunction());
        assertEquals(CameraFunction.class, layer.getLineJoin().getFunction().getClass());
        assertEquals(IntervalStops.class, layer.getLineJoin().getFunction().getStops().getClass());
        assertEquals(1, ((IntervalStops) layer.getLineJoin().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testLineJoinAsIdentitySourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-join");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineJoin(property("FeaturePropertyA", Stops.<String>identity()))
        );

        // Verify
        assertNotNull(layer.getLineJoin());
        assertNotNull(layer.getLineJoin().getFunction());
        assertEquals(SourceFunction.class, layer.getLineJoin().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineJoin().getFunction()).getProperty());
        assertEquals(IdentityStops.class, layer.getLineJoin().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testLineJoinAsIntervalSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-join");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineJoin(
            property(
              "FeaturePropertyA",
              interval(
                stop(1, lineJoin(LINE_JOIN_BEVEL))
              )
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineJoin());
        assertNotNull(layer.getLineJoin().getFunction());
        assertEquals(SourceFunction.class, layer.getLineJoin().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineJoin().getFunction()).getProperty());
        assertEquals(IntervalStops.class, layer.getLineJoin().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testLineMiterLimitAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-miter-limit");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(lineMiterLimit(0.3f));
        assertEquals((Float) layer.getLineMiterLimit().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testLineMiterLimitAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-miter-limit");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineMiterLimit(
            zoom(
              exponential(
                stop(2, lineMiterLimit(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineMiterLimit());
        assertNotNull(layer.getLineMiterLimit().getFunction());
        assertEquals(CameraFunction.class, layer.getLineMiterLimit().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getLineMiterLimit().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getLineMiterLimit().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getLineMiterLimit().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testLineRoundLimitAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-round-limit");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(lineRoundLimit(0.3f));
        assertEquals((Float) layer.getLineRoundLimit().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testLineRoundLimitAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-round-limit");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineRoundLimit(
            zoom(
              exponential(
                stop(2, lineRoundLimit(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineRoundLimit());
        assertNotNull(layer.getLineRoundLimit().getFunction());
        assertEquals(CameraFunction.class, layer.getLineRoundLimit().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getLineRoundLimit().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getLineRoundLimit().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getLineRoundLimit().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testLineOpacityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-opacityTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setLineOpacityTransition(options);
        assertEquals(layer.getLineOpacityTransition(), options);
      }
    });
  }

  @Test
  public void testLineOpacityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(lineOpacity(0.3f));
        assertEquals((Float) layer.getLineOpacity().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testLineOpacityAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineOpacity(
            zoom(
              exponential(
                stop(2, lineOpacity(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineOpacity());
        assertNotNull(layer.getLineOpacity().getFunction());
        assertEquals(CameraFunction.class, layer.getLineOpacity().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getLineOpacity().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getLineOpacity().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getLineOpacity().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testLineOpacityAsIdentitySourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineOpacity(property("FeaturePropertyA", Stops.<Float>identity()))
        );

        // Verify
        assertNotNull(layer.getLineOpacity());
        assertNotNull(layer.getLineOpacity().getFunction());
        assertEquals(SourceFunction.class, layer.getLineOpacity().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineOpacity().getFunction()).getProperty());
        assertEquals(IdentityStops.class, layer.getLineOpacity().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testLineOpacityAsExponentialSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineOpacity(
            property(
              "FeaturePropertyA",
              exponential(
                stop(0.3f, lineOpacity(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineOpacity());
        assertNotNull(layer.getLineOpacity().getFunction());
        assertEquals(SourceFunction.class, layer.getLineOpacity().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineOpacity().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getLineOpacity().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testLineOpacityAsCategoricalSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineOpacity(
            property(
              "FeaturePropertyA",
              categorical(
                stop(1.0f, lineOpacity(0.3f))
              )
            ).withDefaultValue(lineOpacity(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getLineOpacity());
        assertNotNull(layer.getLineOpacity().getFunction());
        assertEquals(SourceFunction.class, layer.getLineOpacity().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineOpacity().getFunction()).getProperty());
        assertEquals(CategoricalStops.class, layer.getLineOpacity().getFunction().getStops().getClass());
        assertNotNull(((SourceFunction) layer.getLineOpacity().getFunction()).getDefaultValue());
        assertNotNull(((SourceFunction) layer.getLineOpacity().getFunction()).getDefaultValue().getValue());
        assertEquals(0.3f, ((SourceFunction) layer.getLineOpacity().getFunction()).getDefaultValue().getValue());
      }
    });

  }

  @Test
  public void testLineOpacityAsCompositeFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-opacity");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineOpacity(
            composite(
              "FeaturePropertyA",
              exponential(
                stop(0, 0.3f, lineOpacity(0.9f))
              ).withBase(0.5f)
            ).withDefaultValue(lineOpacity(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getLineOpacity());
        assertNotNull(layer.getLineOpacity().getFunction());
        assertEquals(CompositeFunction.class, layer.getLineOpacity().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getLineOpacity().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getLineOpacity().getFunction().getStops().getClass());
        assertEquals(1, ((ExponentialStops) layer.getLineOpacity().getFunction().getStops()).size());

        ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
          (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getLineOpacity().getFunction().getStops();
        Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
        assertEquals(0f, stop.in.zoom, 0.001);
        assertEquals(0.3f, stop.in.value, 0.001f);
        assertEquals(0.9f, stop.out, 0.001f);
      }
    });
  }

  @Test
  public void testLineColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-colorTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setLineColorTransition(options);
        assertEquals(layer.getLineColorTransition(), options);
      }
    });
  }

  @Test
  public void testLineColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(lineColor("rgba(0, 0, 0, 1)"));
        assertEquals((String) layer.getLineColor().getValue(), (String) "rgba(0, 0, 0, 1)");
      }
    });
  }

  @Test
  public void testLineColorAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineColor(
            zoom(
              exponential(
                stop(2, lineColor("rgba(0, 0, 0, 1)"))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineColor());
        assertNotNull(layer.getLineColor().getFunction());
        assertEquals(CameraFunction.class, layer.getLineColor().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getLineColor().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getLineColor().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getLineColor().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testLineColorAsIdentitySourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineColor(property("FeaturePropertyA", Stops.<String>identity()))
        );

        // Verify
        assertNotNull(layer.getLineColor());
        assertNotNull(layer.getLineColor().getFunction());
        assertEquals(SourceFunction.class, layer.getLineColor().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineColor().getFunction()).getProperty());
        assertEquals(IdentityStops.class, layer.getLineColor().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testLineColorAsExponentialSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineColor(
            property(
              "FeaturePropertyA",
              exponential(
                stop(Color.RED, lineColor(Color.RED))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineColor());
        assertNotNull(layer.getLineColor().getFunction());
        assertEquals(SourceFunction.class, layer.getLineColor().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineColor().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getLineColor().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testLineColorAsCategoricalSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineColor(
            property(
              "FeaturePropertyA",
              categorical(
                stop("valueA", lineColor(Color.RED))
              )
            ).withDefaultValue(lineColor(Color.GREEN))
          )
        );

        // Verify
        assertNotNull(layer.getLineColor());
        assertNotNull(layer.getLineColor().getFunction());
        assertEquals(SourceFunction.class, layer.getLineColor().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineColor().getFunction()).getProperty());
        assertEquals(CategoricalStops.class, layer.getLineColor().getFunction().getStops().getClass());
        assertNotNull(((SourceFunction) layer.getLineColor().getFunction()).getDefaultValue());
        assertNotNull(((SourceFunction) layer.getLineColor().getFunction()).getDefaultValue().getValue());
        assertEquals(Color.GREEN, (int) ((SourceFunction) layer.getLineColor().getFunction()).getDefaultValue().getColorInt());
      }
    });

  }

  @Test
  public void testLineColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-color");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(lineColor(Color.RED));
        assertEquals(layer.getLineColorAsInt(), Color.RED);
      }
    });
  }

  @Test
  public void testLineTranslateTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-translateTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setLineTranslateTransition(options);
        assertEquals(layer.getLineTranslateTransition(), options);
      }
    });
  }

  @Test
  public void testLineTranslateAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-translate");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(lineTranslate(new Float[] {0f, 0f}));
        assertEquals((Float[]) layer.getLineTranslate().getValue(), (Float[]) new Float[] {0f, 0f});
      }
    });
  }

  @Test
  public void testLineTranslateAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-translate");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineTranslate(
            zoom(
              exponential(
                stop(2, lineTranslate(new Float[] {0f, 0f}))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineTranslate());
        assertNotNull(layer.getLineTranslate().getFunction());
        assertEquals(CameraFunction.class, layer.getLineTranslate().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getLineTranslate().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getLineTranslate().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getLineTranslate().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testLineTranslateAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-translate-anchor");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(lineTranslateAnchor(LINE_TRANSLATE_ANCHOR_MAP));
        assertEquals((String) layer.getLineTranslateAnchor().getValue(), (String) LINE_TRANSLATE_ANCHOR_MAP);
      }
    });
  }

  @Test
  public void testLineTranslateAnchorAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-translate-anchor");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineTranslateAnchor(
            zoom(
              interval(
                stop(2, lineTranslateAnchor(LINE_TRANSLATE_ANCHOR_MAP))
              )
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineTranslateAnchor());
        assertNotNull(layer.getLineTranslateAnchor().getFunction());
        assertEquals(CameraFunction.class, layer.getLineTranslateAnchor().getFunction().getClass());
        assertEquals(IntervalStops.class, layer.getLineTranslateAnchor().getFunction().getStops().getClass());
        assertEquals(1, ((IntervalStops) layer.getLineTranslateAnchor().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testLineWidthTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-widthTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setLineWidthTransition(options);
        assertEquals(layer.getLineWidthTransition(), options);
      }
    });
  }

  @Test
  public void testLineWidthAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-width");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(lineWidth(0.3f));
        assertEquals((Float) layer.getLineWidth().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testLineWidthAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-width");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineWidth(
            zoom(
              exponential(
                stop(2, lineWidth(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineWidth());
        assertNotNull(layer.getLineWidth().getFunction());
        assertEquals(CameraFunction.class, layer.getLineWidth().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getLineWidth().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getLineWidth().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getLineWidth().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testLineWidthAsIdentitySourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-width");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineWidth(property("FeaturePropertyA", Stops.<Float>identity()))
        );

        // Verify
        assertNotNull(layer.getLineWidth());
        assertNotNull(layer.getLineWidth().getFunction());
        assertEquals(SourceFunction.class, layer.getLineWidth().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineWidth().getFunction()).getProperty());
        assertEquals(IdentityStops.class, layer.getLineWidth().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testLineWidthAsExponentialSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-width");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineWidth(
            property(
              "FeaturePropertyA",
              exponential(
                stop(0.3f, lineWidth(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineWidth());
        assertNotNull(layer.getLineWidth().getFunction());
        assertEquals(SourceFunction.class, layer.getLineWidth().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineWidth().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getLineWidth().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testLineWidthAsCategoricalSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-width");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineWidth(
            property(
              "FeaturePropertyA",
              categorical(
                stop(1.0f, lineWidth(0.3f))
              )
            ).withDefaultValue(lineWidth(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getLineWidth());
        assertNotNull(layer.getLineWidth().getFunction());
        assertEquals(SourceFunction.class, layer.getLineWidth().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineWidth().getFunction()).getProperty());
        assertEquals(CategoricalStops.class, layer.getLineWidth().getFunction().getStops().getClass());
        assertNotNull(((SourceFunction) layer.getLineWidth().getFunction()).getDefaultValue());
        assertNotNull(((SourceFunction) layer.getLineWidth().getFunction()).getDefaultValue().getValue());
        assertEquals(0.3f, ((SourceFunction) layer.getLineWidth().getFunction()).getDefaultValue().getValue());
      }
    });

  }

  @Test
  public void testLineWidthAsCompositeFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-width");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineWidth(
            composite(
              "FeaturePropertyA",
              exponential(
                stop(0, 0.3f, lineWidth(0.9f))
              ).withBase(0.5f)
            ).withDefaultValue(lineWidth(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getLineWidth());
        assertNotNull(layer.getLineWidth().getFunction());
        assertEquals(CompositeFunction.class, layer.getLineWidth().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getLineWidth().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getLineWidth().getFunction().getStops().getClass());
        assertEquals(1, ((ExponentialStops) layer.getLineWidth().getFunction().getStops()).size());

        ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
          (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getLineWidth().getFunction().getStops();
        Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
        assertEquals(0f, stop.in.zoom, 0.001);
        assertEquals(0.3f, stop.in.value, 0.001f);
        assertEquals(0.9f, stop.out, 0.001f);
      }
    });
  }

  @Test
  public void testLineGapWidthTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-gap-widthTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setLineGapWidthTransition(options);
        assertEquals(layer.getLineGapWidthTransition(), options);
      }
    });
  }

  @Test
  public void testLineGapWidthAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-gap-width");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(lineGapWidth(0.3f));
        assertEquals((Float) layer.getLineGapWidth().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testLineGapWidthAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-gap-width");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineGapWidth(
            zoom(
              exponential(
                stop(2, lineGapWidth(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineGapWidth());
        assertNotNull(layer.getLineGapWidth().getFunction());
        assertEquals(CameraFunction.class, layer.getLineGapWidth().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getLineGapWidth().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getLineGapWidth().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getLineGapWidth().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testLineGapWidthAsIdentitySourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-gap-width");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineGapWidth(property("FeaturePropertyA", Stops.<Float>identity()))
        );

        // Verify
        assertNotNull(layer.getLineGapWidth());
        assertNotNull(layer.getLineGapWidth().getFunction());
        assertEquals(SourceFunction.class, layer.getLineGapWidth().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineGapWidth().getFunction()).getProperty());
        assertEquals(IdentityStops.class, layer.getLineGapWidth().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testLineGapWidthAsExponentialSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-gap-width");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineGapWidth(
            property(
              "FeaturePropertyA",
              exponential(
                stop(0.3f, lineGapWidth(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineGapWidth());
        assertNotNull(layer.getLineGapWidth().getFunction());
        assertEquals(SourceFunction.class, layer.getLineGapWidth().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineGapWidth().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getLineGapWidth().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testLineGapWidthAsCategoricalSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-gap-width");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineGapWidth(
            property(
              "FeaturePropertyA",
              categorical(
                stop(1.0f, lineGapWidth(0.3f))
              )
            ).withDefaultValue(lineGapWidth(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getLineGapWidth());
        assertNotNull(layer.getLineGapWidth().getFunction());
        assertEquals(SourceFunction.class, layer.getLineGapWidth().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineGapWidth().getFunction()).getProperty());
        assertEquals(CategoricalStops.class, layer.getLineGapWidth().getFunction().getStops().getClass());
        assertNotNull(((SourceFunction) layer.getLineGapWidth().getFunction()).getDefaultValue());
        assertNotNull(((SourceFunction) layer.getLineGapWidth().getFunction()).getDefaultValue().getValue());
        assertEquals(0.3f, ((SourceFunction) layer.getLineGapWidth().getFunction()).getDefaultValue().getValue());
      }
    });

  }

  @Test
  public void testLineGapWidthAsCompositeFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-gap-width");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineGapWidth(
            composite(
              "FeaturePropertyA",
              exponential(
                stop(0, 0.3f, lineGapWidth(0.9f))
              ).withBase(0.5f)
            ).withDefaultValue(lineGapWidth(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getLineGapWidth());
        assertNotNull(layer.getLineGapWidth().getFunction());
        assertEquals(CompositeFunction.class, layer.getLineGapWidth().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getLineGapWidth().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getLineGapWidth().getFunction().getStops().getClass());
        assertEquals(1, ((ExponentialStops) layer.getLineGapWidth().getFunction().getStops()).size());

        ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
          (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getLineGapWidth().getFunction().getStops();
        Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
        assertEquals(0f, stop.in.zoom, 0.001);
        assertEquals(0.3f, stop.in.value, 0.001f);
        assertEquals(0.9f, stop.out, 0.001f);
      }
    });
  }

  @Test
  public void testLineOffsetTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-offsetTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setLineOffsetTransition(options);
        assertEquals(layer.getLineOffsetTransition(), options);
      }
    });
  }

  @Test
  public void testLineOffsetAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-offset");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(lineOffset(0.3f));
        assertEquals((Float) layer.getLineOffset().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testLineOffsetAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-offset");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineOffset(
            zoom(
              exponential(
                stop(2, lineOffset(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineOffset());
        assertNotNull(layer.getLineOffset().getFunction());
        assertEquals(CameraFunction.class, layer.getLineOffset().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getLineOffset().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getLineOffset().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getLineOffset().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testLineOffsetAsIdentitySourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-offset");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineOffset(property("FeaturePropertyA", Stops.<Float>identity()))
        );

        // Verify
        assertNotNull(layer.getLineOffset());
        assertNotNull(layer.getLineOffset().getFunction());
        assertEquals(SourceFunction.class, layer.getLineOffset().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineOffset().getFunction()).getProperty());
        assertEquals(IdentityStops.class, layer.getLineOffset().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testLineOffsetAsExponentialSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-offset");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineOffset(
            property(
              "FeaturePropertyA",
              exponential(
                stop(0.3f, lineOffset(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineOffset());
        assertNotNull(layer.getLineOffset().getFunction());
        assertEquals(SourceFunction.class, layer.getLineOffset().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineOffset().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getLineOffset().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testLineOffsetAsCategoricalSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-offset");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineOffset(
            property(
              "FeaturePropertyA",
              categorical(
                stop(1.0f, lineOffset(0.3f))
              )
            ).withDefaultValue(lineOffset(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getLineOffset());
        assertNotNull(layer.getLineOffset().getFunction());
        assertEquals(SourceFunction.class, layer.getLineOffset().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineOffset().getFunction()).getProperty());
        assertEquals(CategoricalStops.class, layer.getLineOffset().getFunction().getStops().getClass());
        assertNotNull(((SourceFunction) layer.getLineOffset().getFunction()).getDefaultValue());
        assertNotNull(((SourceFunction) layer.getLineOffset().getFunction()).getDefaultValue().getValue());
        assertEquals(0.3f, ((SourceFunction) layer.getLineOffset().getFunction()).getDefaultValue().getValue());
      }
    });

  }

  @Test
  public void testLineOffsetAsCompositeFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-offset");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineOffset(
            composite(
              "FeaturePropertyA",
              exponential(
                stop(0, 0.3f, lineOffset(0.9f))
              ).withBase(0.5f)
            ).withDefaultValue(lineOffset(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getLineOffset());
        assertNotNull(layer.getLineOffset().getFunction());
        assertEquals(CompositeFunction.class, layer.getLineOffset().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getLineOffset().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getLineOffset().getFunction().getStops().getClass());
        assertEquals(1, ((ExponentialStops) layer.getLineOffset().getFunction().getStops()).size());

        ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
          (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getLineOffset().getFunction().getStops();
        Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
        assertEquals(0f, stop.in.zoom, 0.001);
        assertEquals(0.3f, stop.in.value, 0.001f);
        assertEquals(0.9f, stop.out, 0.001f);
      }
    });
  }

  @Test
  public void testLineBlurTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-blurTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setLineBlurTransition(options);
        assertEquals(layer.getLineBlurTransition(), options);
      }
    });
  }

  @Test
  public void testLineBlurAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-blur");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(lineBlur(0.3f));
        assertEquals((Float) layer.getLineBlur().getValue(), (Float) 0.3f);
      }
    });
  }

  @Test
  public void testLineBlurAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-blur");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineBlur(
            zoom(
              exponential(
                stop(2, lineBlur(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineBlur());
        assertNotNull(layer.getLineBlur().getFunction());
        assertEquals(CameraFunction.class, layer.getLineBlur().getFunction().getClass());
        assertEquals(ExponentialStops.class, layer.getLineBlur().getFunction().getStops().getClass());
        assertEquals(0.5f, ((ExponentialStops) layer.getLineBlur().getFunction().getStops()).getBase(), 0.001);
        assertEquals(1, ((ExponentialStops) layer.getLineBlur().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testLineBlurAsIdentitySourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-blur");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineBlur(property("FeaturePropertyA", Stops.<Float>identity()))
        );

        // Verify
        assertNotNull(layer.getLineBlur());
        assertNotNull(layer.getLineBlur().getFunction());
        assertEquals(SourceFunction.class, layer.getLineBlur().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineBlur().getFunction()).getProperty());
        assertEquals(IdentityStops.class, layer.getLineBlur().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testLineBlurAsExponentialSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-blur");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineBlur(
            property(
              "FeaturePropertyA",
              exponential(
                stop(0.3f, lineBlur(0.3f))
              ).withBase(0.5f)
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineBlur());
        assertNotNull(layer.getLineBlur().getFunction());
        assertEquals(SourceFunction.class, layer.getLineBlur().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineBlur().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getLineBlur().getFunction().getStops().getClass());
      }
    });
  }

  @Test
  public void testLineBlurAsCategoricalSourceFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-blur");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineBlur(
            property(
              "FeaturePropertyA",
              categorical(
                stop(1.0f, lineBlur(0.3f))
              )
            ).withDefaultValue(lineBlur(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getLineBlur());
        assertNotNull(layer.getLineBlur().getFunction());
        assertEquals(SourceFunction.class, layer.getLineBlur().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((SourceFunction) layer.getLineBlur().getFunction()).getProperty());
        assertEquals(CategoricalStops.class, layer.getLineBlur().getFunction().getStops().getClass());
        assertNotNull(((SourceFunction) layer.getLineBlur().getFunction()).getDefaultValue());
        assertNotNull(((SourceFunction) layer.getLineBlur().getFunction()).getDefaultValue().getValue());
        assertEquals(0.3f, ((SourceFunction) layer.getLineBlur().getFunction()).getDefaultValue().getValue());
      }
    });

  }

  @Test
  public void testLineBlurAsCompositeFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-blur");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineBlur(
            composite(
              "FeaturePropertyA",
              exponential(
                stop(0, 0.3f, lineBlur(0.9f))
              ).withBase(0.5f)
            ).withDefaultValue(lineBlur(0.3f))
          )
        );

        // Verify
        assertNotNull(layer.getLineBlur());
        assertNotNull(layer.getLineBlur().getFunction());
        assertEquals(CompositeFunction.class, layer.getLineBlur().getFunction().getClass());
        assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getLineBlur().getFunction()).getProperty());
        assertEquals(ExponentialStops.class, layer.getLineBlur().getFunction().getStops().getClass());
        assertEquals(1, ((ExponentialStops) layer.getLineBlur().getFunction().getStops()).size());

        ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
          (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getLineBlur().getFunction().getStops();
        Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
        assertEquals(0f, stop.in.zoom, 0.001);
        assertEquals(0.3f, stop.in.value, 0.001f);
        assertEquals(0.9f, stop.out, 0.001f);
      }
    });
  }

  @Test
  public void testLineDasharrayTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-dasharrayTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setLineDasharrayTransition(options);
        assertEquals(layer.getLineDasharrayTransition(), options);
      }
    });
  }

  @Test
  public void testLineDasharrayAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-dasharray");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(lineDasharray(new Float[] {}));
        assertEquals((Float[]) layer.getLineDasharray().getValue(), (Float[]) new Float[] {});
      }
    });
  }

  @Test
  public void testLineDasharrayAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-dasharray");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          lineDasharray(
            zoom(
              interval(
                stop(2, lineDasharray(new Float[] {}))
              )
            )
          )
        );

        // Verify
        assertNotNull(layer.getLineDasharray());
        assertNotNull(layer.getLineDasharray().getFunction());
        assertEquals(CameraFunction.class, layer.getLineDasharray().getFunction().getClass());
        assertEquals(IntervalStops.class, layer.getLineDasharray().getFunction().getStops().getClass());
        assertEquals(1, ((IntervalStops) layer.getLineDasharray().getFunction().getStops()).size());
      }
    });
  }

  @Test
  public void testLinePatternTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-patternTransitionOptions");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        TransitionOptions options = new TransitionOptions(300, 100);
        layer.setLinePatternTransition(options);
        assertEquals(layer.getLinePatternTransition(), options);
      }
    });
  }

  @Test
  public void testLinePatternAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-pattern");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set and Get
        layer.setProperties(linePattern("pedestrian-polygon"));
        assertEquals((String) layer.getLinePattern().getValue(), (String) "pedestrian-polygon");
      }
    });
  }

  @Test
  public void testLinePatternAsCameraFunction() {
    validateTestSetup();
    setupLayer();
    Timber.i("line-pattern");
    invoke(mapboxMap, new MapboxMapAction.OnInvokeActionListener() {
      @Override
      public void onInvokeAction(UiController uiController, MapboxMap mapboxMap) {
        assertNotNull(layer);

        // Set
        layer.setProperties(
          linePattern(
            zoom(
              interval(
                stop(2, linePattern("pedestrian-polygon"))
              )
            )
          )
        );

        // Verify
        assertNotNull(layer.getLinePattern());
        assertNotNull(layer.getLinePattern().getFunction());
        assertEquals(CameraFunction.class, layer.getLinePattern().getFunction().getClass());
        assertEquals(IntervalStops.class, layer.getLinePattern().getFunction().getStops().getClass());
        assertEquals(1, ((IntervalStops) layer.getLinePattern().getFunction().getStops()).size());
      }
    });
  }

}
