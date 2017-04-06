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
import com.mapbox.mapboxsdk.style.functions.stops.ExponentialStops;
import com.mapbox.mapboxsdk.style.functions.stops.IdentityStops;
import com.mapbox.mapboxsdk.style.functions.stops.IntervalStops;
import com.mapbox.mapboxsdk.style.functions.stops.Stop;
import com.mapbox.mapboxsdk.style.functions.stops.Stops;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import static com.mapbox.mapboxsdk.style.functions.Function.*;
import static com.mapbox.mapboxsdk.style.functions.stops.Stop.stop;
import static com.mapbox.mapboxsdk.style.functions.stops.Stops.*;
import static org.junit.Assert.*;
import static com.mapbox.mapboxsdk.style.layers.Property.*;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.*;

import com.mapbox.mapboxsdk.style.layers.TransitionOptions;

/**
 * Basic smoke tests for SymbolLayer
 */
@RunWith(AndroidJUnit4.class)
public class SymbolLayerTest extends BaseStyleTest {

  @Rule
  public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

  private SymbolLayer layer;

  private OnMapReadyIdlingResource idlingResource;

  private MapboxMap mapboxMap;

  @Before
  public void setup() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
    mapboxMap = rule.getActivity().getMapboxMap();

    if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
      Timber.i("Adding layer");
      layer = new SymbolLayer("my-layer", "composite");
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
  public void testSymbolPlacementAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("symbol-placement");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(symbolPlacement(SYMBOL_PLACEMENT_POINT));
    assertEquals((String) layer.getSymbolPlacement().getValue(), (String) SYMBOL_PLACEMENT_POINT);
  }

  @Test
  public void testSymbolPlacementAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("symbol-placement");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      symbolPlacement(
        zoom(
          interval(
            stop(2, symbolPlacement(SYMBOL_PLACEMENT_POINT))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getSymbolPlacement());
    assertNotNull(layer.getSymbolPlacement().getFunction());
    assertEquals(CameraFunction.class, layer.getSymbolPlacement().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getSymbolPlacement().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getSymbolPlacement().getFunction().getStops()).size());
  }


  @Test
  public void testSymbolSpacingAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("symbol-spacing");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(symbolSpacing(0.3f));
    assertEquals((Float) layer.getSymbolSpacing().getValue(), (Float) 0.3f);
  }

  @Test
  public void testSymbolSpacingAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("symbol-spacing");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      symbolSpacing(
        zoom(
          exponential(
            stop(2, symbolSpacing(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getSymbolSpacing());
    assertNotNull(layer.getSymbolSpacing().getFunction());
    assertEquals(CameraFunction.class, layer.getSymbolSpacing().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getSymbolSpacing().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getSymbolSpacing().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getSymbolSpacing().getFunction().getStops()).size());
  }


  @Test
  public void testSymbolAvoidEdgesAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("symbol-avoid-edges");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(symbolAvoidEdges(true));
    assertEquals((Boolean) layer.getSymbolAvoidEdges().getValue(), (Boolean) true);
  }

  @Test
  public void testSymbolAvoidEdgesAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("symbol-avoid-edges");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      symbolAvoidEdges(
        zoom(
          interval(
            stop(2, symbolAvoidEdges(true))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getSymbolAvoidEdges());
    assertNotNull(layer.getSymbolAvoidEdges().getFunction());
    assertEquals(CameraFunction.class, layer.getSymbolAvoidEdges().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getSymbolAvoidEdges().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getSymbolAvoidEdges().getFunction().getStops()).size());
  }


  @Test
  public void testIconAllowOverlapAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-allow-overlap");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconAllowOverlap(true));
    assertEquals((Boolean) layer.getIconAllowOverlap().getValue(), (Boolean) true);
  }

  @Test
  public void testIconAllowOverlapAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-allow-overlap");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconAllowOverlap(
        zoom(
          interval(
            stop(2, iconAllowOverlap(true))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconAllowOverlap());
    assertNotNull(layer.getIconAllowOverlap().getFunction());
    assertEquals(CameraFunction.class, layer.getIconAllowOverlap().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getIconAllowOverlap().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getIconAllowOverlap().getFunction().getStops()).size());
  }


  @Test
  public void testIconIgnorePlacementAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-ignore-placement");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconIgnorePlacement(true));
    assertEquals((Boolean) layer.getIconIgnorePlacement().getValue(), (Boolean) true);
  }

  @Test
  public void testIconIgnorePlacementAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-ignore-placement");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconIgnorePlacement(
        zoom(
          interval(
            stop(2, iconIgnorePlacement(true))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconIgnorePlacement());
    assertNotNull(layer.getIconIgnorePlacement().getFunction());
    assertEquals(CameraFunction.class, layer.getIconIgnorePlacement().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getIconIgnorePlacement().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getIconIgnorePlacement().getFunction().getStops()).size());
  }


  @Test
  public void testIconOptionalAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-optional");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconOptional(true));
    assertEquals((Boolean) layer.getIconOptional().getValue(), (Boolean) true);
  }

  @Test
  public void testIconOptionalAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-optional");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconOptional(
        zoom(
          interval(
            stop(2, iconOptional(true))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconOptional());
    assertNotNull(layer.getIconOptional().getFunction());
    assertEquals(CameraFunction.class, layer.getIconOptional().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getIconOptional().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getIconOptional().getFunction().getStops()).size());
  }


  @Test
  public void testIconRotationAlignmentAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-rotation-alignment");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconRotationAlignment(ICON_ROTATION_ALIGNMENT_MAP));
    assertEquals((String) layer.getIconRotationAlignment().getValue(), (String) ICON_ROTATION_ALIGNMENT_MAP);
  }

  @Test
  public void testIconRotationAlignmentAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-rotation-alignment");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconRotationAlignment(
        zoom(
          interval(
            stop(2, iconRotationAlignment(ICON_ROTATION_ALIGNMENT_MAP))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconRotationAlignment());
    assertNotNull(layer.getIconRotationAlignment().getFunction());
    assertEquals(CameraFunction.class, layer.getIconRotationAlignment().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getIconRotationAlignment().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getIconRotationAlignment().getFunction().getStops()).size());
  }


  @Test
  public void testIconSizeAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-size");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconSize(0.3f));
    assertEquals((Float) layer.getIconSize().getValue(), (Float) 0.3f);
  }

  @Test
  public void testIconSizeAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-size");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconSize(
        zoom(
          exponential(
            stop(2, iconSize(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconSize());
    assertNotNull(layer.getIconSize().getFunction());
    assertEquals(CameraFunction.class, layer.getIconSize().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getIconSize().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getIconSize().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getIconSize().getFunction().getStops()).size());
  }


  @Test
  public void testIconSizeAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-size");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconSize(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getIconSize());
    assertNotNull(layer.getIconSize().getFunction());
    assertEquals(SourceFunction.class, layer.getIconSize().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconSize().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getIconSize().getFunction().getStops().getClass());
  }

  @Test
  public void testIconSizeAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-size");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconSize(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, iconSize(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconSize());
    assertNotNull(layer.getIconSize().getFunction());
    assertEquals(SourceFunction.class, layer.getIconSize().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconSize().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getIconSize().getFunction().getStops().getClass());
  }

  @Test
  public void testIconSizeAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-size");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconSize(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, iconSize(0.3f))
          )
        ).withDefaultValue(iconSize(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getIconSize());
    assertNotNull(layer.getIconSize().getFunction());
    assertEquals(SourceFunction.class, layer.getIconSize().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconSize().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getIconSize().getFunction().getStops().getClass());
    assertNotNull(((SourceFunction) layer.getIconSize().getFunction()).getDefaultValue());
    assertNotNull(((SourceFunction) layer.getIconSize().getFunction()).getDefaultValue().getValue());
    assertEquals(0.3f, ((SourceFunction) layer.getIconSize().getFunction()).getDefaultValue().getValue());
  }

  @Test
  public void testIconSizeAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-size");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconSize(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, iconSize(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(iconSize(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getIconSize());
    assertNotNull(layer.getIconSize().getFunction());
    assertEquals(CompositeFunction.class, layer.getIconSize().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getIconSize().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getIconSize().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getIconSize().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getIconSize().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testIconTextFitAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-text-fit");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconTextFit(ICON_TEXT_FIT_NONE));
    assertEquals((String) layer.getIconTextFit().getValue(), (String) ICON_TEXT_FIT_NONE);
  }

  @Test
  public void testIconTextFitAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-text-fit");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconTextFit(
        zoom(
          interval(
            stop(2, iconTextFit(ICON_TEXT_FIT_NONE))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconTextFit());
    assertNotNull(layer.getIconTextFit().getFunction());
    assertEquals(CameraFunction.class, layer.getIconTextFit().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getIconTextFit().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getIconTextFit().getFunction().getStops()).size());
  }


  @Test
  public void testIconTextFitPaddingAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-text-fit-padding");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconTextFitPadding(new Float[]{0f,0f,0f,0f}));
    assertEquals((Float[]) layer.getIconTextFitPadding().getValue(), (Float[]) new Float[]{0f,0f,0f,0f});
  }

  @Test
  public void testIconTextFitPaddingAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-text-fit-padding");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconTextFitPadding(
        zoom(
          exponential(
            stop(2, iconTextFitPadding(new Float[]{0f,0f,0f,0f}))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconTextFitPadding());
    assertNotNull(layer.getIconTextFitPadding().getFunction());
    assertEquals(CameraFunction.class, layer.getIconTextFitPadding().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getIconTextFitPadding().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getIconTextFitPadding().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getIconTextFitPadding().getFunction().getStops()).size());
  }


  @Test
  public void testIconImageAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-image");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconImage("undefined"));
    assertEquals((String) layer.getIconImage().getValue(), (String) "undefined");
  }

  @Test
  public void testIconImageAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-image");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconImage(
        zoom(
          interval(
            stop(2, iconImage("undefined"))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconImage());
    assertNotNull(layer.getIconImage().getFunction());
    assertEquals(CameraFunction.class, layer.getIconImage().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getIconImage().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getIconImage().getFunction().getStops()).size());
  }


  @Test
  public void testIconImageAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-image");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconImage(property("FeaturePropertyA", Stops.<String>identity()))
    );

    // Verify
    assertNotNull(layer.getIconImage());
    assertNotNull(layer.getIconImage().getFunction());
    assertEquals(SourceFunction.class, layer.getIconImage().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconImage().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getIconImage().getFunction().getStops().getClass());
  }

  @Test
  public void testIconImageAsIntervalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-image");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconImage(
        property(
          "FeaturePropertyA",
          interval(
            stop(1, iconImage("undefined"))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconImage());
    assertNotNull(layer.getIconImage().getFunction());
    assertEquals(SourceFunction.class, layer.getIconImage().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconImage().getFunction()).getProperty());
    assertEquals(IntervalStops.class, layer.getIconImage().getFunction().getStops().getClass());
  }

  @Test
  public void testIconRotateAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-rotate");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconRotate(0.3f));
    assertEquals((Float) layer.getIconRotate().getValue(), (Float) 0.3f);
  }

  @Test
  public void testIconRotateAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-rotate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconRotate(
        zoom(
          exponential(
            stop(2, iconRotate(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconRotate());
    assertNotNull(layer.getIconRotate().getFunction());
    assertEquals(CameraFunction.class, layer.getIconRotate().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getIconRotate().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getIconRotate().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getIconRotate().getFunction().getStops()).size());
  }

  @Test
  public void testIconRotateAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-rotate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconRotate(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getIconRotate());
    assertNotNull(layer.getIconRotate().getFunction());
    assertEquals(SourceFunction.class, layer.getIconRotate().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconRotate().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getIconRotate().getFunction().getStops().getClass());
  }

  @Test
  public void testIconRotateAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-rotate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconRotate(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, iconRotate(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconRotate());
    assertNotNull(layer.getIconRotate().getFunction());
    assertEquals(SourceFunction.class, layer.getIconRotate().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconRotate().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getIconRotate().getFunction().getStops().getClass());
  }

  @Test
  public void testIconRotateAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-rotate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconRotate(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, iconRotate(0.3f))
          )
        ).withDefaultValue(iconRotate(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getIconRotate());
    assertNotNull(layer.getIconRotate().getFunction());
    assertEquals(SourceFunction.class, layer.getIconRotate().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconRotate().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getIconRotate().getFunction().getStops().getClass());
    assertNotNull(((SourceFunction) layer.getIconRotate().getFunction()).getDefaultValue());
    assertNotNull(((SourceFunction) layer.getIconRotate().getFunction()).getDefaultValue().getValue());
    assertEquals(0.3f, ((SourceFunction) layer.getIconRotate().getFunction()).getDefaultValue().getValue());
  }

  @Test
  public void testIconRotateAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-rotate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconRotate(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, iconRotate(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(iconRotate(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getIconRotate());
    assertNotNull(layer.getIconRotate().getFunction());
    assertEquals(CompositeFunction.class, layer.getIconRotate().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getIconRotate().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getIconRotate().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getIconRotate().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getIconRotate().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }


  @Test
  public void testIconPaddingAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-padding");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconPadding(0.3f));
    assertEquals((Float) layer.getIconPadding().getValue(), (Float) 0.3f);
  }

  @Test
  public void testIconPaddingAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-padding");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconPadding(
        zoom(
          exponential(
            stop(2, iconPadding(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconPadding());
    assertNotNull(layer.getIconPadding().getFunction());
    assertEquals(CameraFunction.class, layer.getIconPadding().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getIconPadding().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getIconPadding().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getIconPadding().getFunction().getStops()).size());
  }


  @Test
  public void testIconKeepUprightAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-keep-upright");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconKeepUpright(true));
    assertEquals((Boolean) layer.getIconKeepUpright().getValue(), (Boolean) true);
  }

  @Test
  public void testIconKeepUprightAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-keep-upright");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconKeepUpright(
        zoom(
          interval(
            stop(2, iconKeepUpright(true))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconKeepUpright());
    assertNotNull(layer.getIconKeepUpright().getFunction());
    assertEquals(CameraFunction.class, layer.getIconKeepUpright().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getIconKeepUpright().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getIconKeepUpright().getFunction().getStops()).size());
  }


  @Test
  public void testIconOffsetAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-offset");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconOffset(new Float[]{0f,0f}));
    assertEquals((Float[]) layer.getIconOffset().getValue(), (Float[]) new Float[]{0f,0f});
  }

  @Test
  public void testIconOffsetAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-offset");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconOffset(
        zoom(
          exponential(
            stop(2, iconOffset(new Float[]{0f,0f}))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconOffset());
    assertNotNull(layer.getIconOffset().getFunction());
    assertEquals(CameraFunction.class, layer.getIconOffset().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getIconOffset().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getIconOffset().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getIconOffset().getFunction().getStops()).size());
  }

  @Test
  public void testIconOffsetAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-offset");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconOffset(property("FeaturePropertyA", Stops.<Float[]>identity()))
    );

    // Verify
    assertNotNull(layer.getIconOffset());
    assertNotNull(layer.getIconOffset().getFunction());
    assertEquals(SourceFunction.class, layer.getIconOffset().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconOffset().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getIconOffset().getFunction().getStops().getClass());
  }

  @Test
  public void testIconOffsetAsIntervalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-offset");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconOffset(
        property(
          "FeaturePropertyA",
          interval(
            stop(1, iconOffset(new Float[]{0f,0f}))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconOffset());
    assertNotNull(layer.getIconOffset().getFunction());
    assertEquals(SourceFunction.class, layer.getIconOffset().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconOffset().getFunction()).getProperty());
    assertEquals(IntervalStops.class, layer.getIconOffset().getFunction().getStops().getClass());
  }


  @Test
  public void testTextPitchAlignmentAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-pitch-alignment");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textPitchAlignment(TEXT_PITCH_ALIGNMENT_MAP));
    assertEquals((String) layer.getTextPitchAlignment().getValue(), (String) TEXT_PITCH_ALIGNMENT_MAP);
  }

  @Test
  public void testTextPitchAlignmentAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-pitch-alignment");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textPitchAlignment(
        zoom(
          interval(
            stop(2, textPitchAlignment(TEXT_PITCH_ALIGNMENT_MAP))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextPitchAlignment());
    assertNotNull(layer.getTextPitchAlignment().getFunction());
    assertEquals(CameraFunction.class, layer.getTextPitchAlignment().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getTextPitchAlignment().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getTextPitchAlignment().getFunction().getStops()).size());
  }


  @Test
  public void testTextRotationAlignmentAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-rotation-alignment");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textRotationAlignment(TEXT_ROTATION_ALIGNMENT_MAP));
    assertEquals((String) layer.getTextRotationAlignment().getValue(), (String) TEXT_ROTATION_ALIGNMENT_MAP);
  }

  @Test
  public void testTextRotationAlignmentAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-rotation-alignment");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textRotationAlignment(
        zoom(
          interval(
            stop(2, textRotationAlignment(TEXT_ROTATION_ALIGNMENT_MAP))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextRotationAlignment());
    assertNotNull(layer.getTextRotationAlignment().getFunction());
    assertEquals(CameraFunction.class, layer.getTextRotationAlignment().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getTextRotationAlignment().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getTextRotationAlignment().getFunction().getStops()).size());
  }


  @Test
  public void testTextFieldAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-field");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textField(""));
    assertEquals((String) layer.getTextField().getValue(), (String) "");
  }

  @Test
  public void testTextFieldAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-field");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textField(
        zoom(
          interval(
            stop(2, textField(""))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextField());
    assertNotNull(layer.getTextField().getFunction());
    assertEquals(CameraFunction.class, layer.getTextField().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getTextField().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getTextField().getFunction().getStops()).size());
  }

  @Test
  public void testTextFieldAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-field");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textField(property("FeaturePropertyA", Stops.<String>identity()))
    );

    // Verify
    assertNotNull(layer.getTextField());
    assertNotNull(layer.getTextField().getFunction());
    assertEquals(SourceFunction.class, layer.getTextField().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextField().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getTextField().getFunction().getStops().getClass());
  }

  @Test
  public void testTextFieldAsIntervalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-field");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textField(
        property(
          "FeaturePropertyA",
          interval(
            stop(1, textField(""))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextField());
    assertNotNull(layer.getTextField().getFunction());
    assertEquals(SourceFunction.class, layer.getTextField().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextField().getFunction()).getProperty());
    assertEquals(IntervalStops.class, layer.getTextField().getFunction().getStops().getClass());
  }


  @Test
  public void testTextFontAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-font");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textFont(new String[]{"Open Sans Regular", "Arial Unicode MS Regular"}));
    assertEquals((String[]) layer.getTextFont().getValue(), (String[]) new String[]{"Open Sans Regular", "Arial Unicode MS Regular"});
  }

  @Test
  public void testTextFontAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-font");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textFont(
        zoom(
          interval(
            stop(2, textFont(new String[]{"Open Sans Regular", "Arial Unicode MS Regular"}))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextFont());
    assertNotNull(layer.getTextFont().getFunction());
    assertEquals(CameraFunction.class, layer.getTextFont().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getTextFont().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getTextFont().getFunction().getStops()).size());
  }


  @Test
  public void testTextSizeAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-size");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textSize(0.3f));
    assertEquals((Float) layer.getTextSize().getValue(), (Float) 0.3f);
  }

  @Test
  public void testTextSizeAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-size");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textSize(
        zoom(
          exponential(
            stop(2, textSize(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextSize());
    assertNotNull(layer.getTextSize().getFunction());
    assertEquals(CameraFunction.class, layer.getTextSize().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getTextSize().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getTextSize().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getTextSize().getFunction().getStops()).size());
  }


  @Test
  public void testTextSizeAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-size");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textSize(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getTextSize());
    assertNotNull(layer.getTextSize().getFunction());
    assertEquals(SourceFunction.class, layer.getTextSize().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextSize().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getTextSize().getFunction().getStops().getClass());
  }

  @Test
  public void testTextSizeAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-size");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textSize(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, textSize(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextSize());
    assertNotNull(layer.getTextSize().getFunction());
    assertEquals(SourceFunction.class, layer.getTextSize().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextSize().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getTextSize().getFunction().getStops().getClass());
  }

  @Test
  public void testTextSizeAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-size");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textSize(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, textSize(0.3f))
          )
        ).withDefaultValue(textSize(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getTextSize());
    assertNotNull(layer.getTextSize().getFunction());
    assertEquals(SourceFunction.class, layer.getTextSize().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextSize().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getTextSize().getFunction().getStops().getClass());
    assertNotNull(((SourceFunction) layer.getTextSize().getFunction()).getDefaultValue());
    assertNotNull(((SourceFunction) layer.getTextSize().getFunction()).getDefaultValue().getValue());
    assertEquals(0.3f, ((SourceFunction) layer.getTextSize().getFunction()).getDefaultValue().getValue());
  }

  @Test
  public void testTextSizeAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-size");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textSize(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, textSize(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(textSize(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getTextSize());
    assertNotNull(layer.getTextSize().getFunction());
    assertEquals(CompositeFunction.class, layer.getTextSize().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getTextSize().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getTextSize().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getTextSize().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getTextSize().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testTextMaxWidthAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-max-width");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textMaxWidth(0.3f));
    assertEquals((Float) layer.getTextMaxWidth().getValue(), (Float) 0.3f);
  }

  @Test
  public void testTextMaxWidthAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-max-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textMaxWidth(
        zoom(
          exponential(
            stop(2, textMaxWidth(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextMaxWidth());
    assertNotNull(layer.getTextMaxWidth().getFunction());
    assertEquals(CameraFunction.class, layer.getTextMaxWidth().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getTextMaxWidth().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getTextMaxWidth().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getTextMaxWidth().getFunction().getStops()).size());
  }


  @Test
  public void testTextLineHeightAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-line-height");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textLineHeight(0.3f));
    assertEquals((Float) layer.getTextLineHeight().getValue(), (Float) 0.3f);
  }

  @Test
  public void testTextLineHeightAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-line-height");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textLineHeight(
        zoom(
          exponential(
            stop(2, textLineHeight(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextLineHeight());
    assertNotNull(layer.getTextLineHeight().getFunction());
    assertEquals(CameraFunction.class, layer.getTextLineHeight().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getTextLineHeight().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getTextLineHeight().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getTextLineHeight().getFunction().getStops()).size());
  }


  @Test
  public void testTextLetterSpacingAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-letter-spacing");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textLetterSpacing(0.3f));
    assertEquals((Float) layer.getTextLetterSpacing().getValue(), (Float) 0.3f);
  }

  @Test
  public void testTextLetterSpacingAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-letter-spacing");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textLetterSpacing(
        zoom(
          exponential(
            stop(2, textLetterSpacing(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextLetterSpacing());
    assertNotNull(layer.getTextLetterSpacing().getFunction());
    assertEquals(CameraFunction.class, layer.getTextLetterSpacing().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getTextLetterSpacing().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getTextLetterSpacing().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getTextLetterSpacing().getFunction().getStops()).size());
  }


  @Test
  public void testTextJustifyAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-justify");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textJustify(TEXT_JUSTIFY_LEFT));
    assertEquals((String) layer.getTextJustify().getValue(), (String) TEXT_JUSTIFY_LEFT);
  }

  @Test
  public void testTextJustifyAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-justify");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textJustify(
        zoom(
          interval(
            stop(2, textJustify(TEXT_JUSTIFY_LEFT))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextJustify());
    assertNotNull(layer.getTextJustify().getFunction());
    assertEquals(CameraFunction.class, layer.getTextJustify().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getTextJustify().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getTextJustify().getFunction().getStops()).size());
  }


  @Test
  public void testTextAnchorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-anchor");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textAnchor(TEXT_ANCHOR_CENTER));
    assertEquals((String) layer.getTextAnchor().getValue(), (String) TEXT_ANCHOR_CENTER);
  }

  @Test
  public void testTextAnchorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-anchor");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textAnchor(
        zoom(
          interval(
            stop(2, textAnchor(TEXT_ANCHOR_CENTER))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextAnchor());
    assertNotNull(layer.getTextAnchor().getFunction());
    assertEquals(CameraFunction.class, layer.getTextAnchor().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getTextAnchor().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getTextAnchor().getFunction().getStops()).size());
  }


  @Test
  public void testTextMaxAngleAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-max-angle");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textMaxAngle(0.3f));
    assertEquals((Float) layer.getTextMaxAngle().getValue(), (Float) 0.3f);
  }

  @Test
  public void testTextMaxAngleAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-max-angle");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textMaxAngle(
        zoom(
          exponential(
            stop(2, textMaxAngle(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextMaxAngle());
    assertNotNull(layer.getTextMaxAngle().getFunction());
    assertEquals(CameraFunction.class, layer.getTextMaxAngle().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getTextMaxAngle().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getTextMaxAngle().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getTextMaxAngle().getFunction().getStops()).size());
  }


  @Test
  public void testTextRotateAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-rotate");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textRotate(0.3f));
    assertEquals((Float) layer.getTextRotate().getValue(), (Float) 0.3f);
  }

  @Test
  public void testTextRotateAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-rotate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textRotate(
        zoom(
          exponential(
            stop(2, textRotate(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextRotate());
    assertNotNull(layer.getTextRotate().getFunction());
    assertEquals(CameraFunction.class, layer.getTextRotate().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getTextRotate().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getTextRotate().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getTextRotate().getFunction().getStops()).size());
  }


  @Test
  public void testTextRotateAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-rotate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textRotate(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getTextRotate());
    assertNotNull(layer.getTextRotate().getFunction());
    assertEquals(SourceFunction.class, layer.getTextRotate().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextRotate().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getTextRotate().getFunction().getStops().getClass());
  }

  @Test
  public void testTextRotateAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-rotate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textRotate(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, textRotate(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextRotate());
    assertNotNull(layer.getTextRotate().getFunction());
    assertEquals(SourceFunction.class, layer.getTextRotate().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextRotate().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getTextRotate().getFunction().getStops().getClass());
  }

  @Test
  public void testTextRotateAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-rotate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textRotate(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, textRotate(0.3f))
          )
        ).withDefaultValue(textRotate(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getTextRotate());
    assertNotNull(layer.getTextRotate().getFunction());
    assertEquals(SourceFunction.class, layer.getTextRotate().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextRotate().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getTextRotate().getFunction().getStops().getClass());
    assertNotNull(((SourceFunction) layer.getTextRotate().getFunction()).getDefaultValue());
    assertNotNull(((SourceFunction) layer.getTextRotate().getFunction()).getDefaultValue().getValue());
    assertEquals(0.3f, ((SourceFunction) layer.getTextRotate().getFunction()).getDefaultValue().getValue());
  }

  @Test
  public void testTextRotateAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-rotate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textRotate(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, textRotate(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(textRotate(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getTextRotate());
    assertNotNull(layer.getTextRotate().getFunction());
    assertEquals(CompositeFunction.class, layer.getTextRotate().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getTextRotate().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getTextRotate().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getTextRotate().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getTextRotate().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testTextPaddingAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-padding");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textPadding(0.3f));
    assertEquals((Float) layer.getTextPadding().getValue(), (Float) 0.3f);
  }

  @Test
  public void testTextPaddingAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-padding");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textPadding(
        zoom(
          exponential(
            stop(2, textPadding(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextPadding());
    assertNotNull(layer.getTextPadding().getFunction());
    assertEquals(CameraFunction.class, layer.getTextPadding().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getTextPadding().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getTextPadding().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getTextPadding().getFunction().getStops()).size());
  }


  @Test
  public void testTextKeepUprightAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-keep-upright");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textKeepUpright(true));
    assertEquals((Boolean) layer.getTextKeepUpright().getValue(), (Boolean) true);
  }

  @Test
  public void testTextKeepUprightAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-keep-upright");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textKeepUpright(
        zoom(
          interval(
            stop(2, textKeepUpright(true))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextKeepUpright());
    assertNotNull(layer.getTextKeepUpright().getFunction());
    assertEquals(CameraFunction.class, layer.getTextKeepUpright().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getTextKeepUpright().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getTextKeepUpright().getFunction().getStops()).size());
  }


  @Test
  public void testTextTransformAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-transform");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textTransform(TEXT_TRANSFORM_NONE));
    assertEquals((String) layer.getTextTransform().getValue(), (String) TEXT_TRANSFORM_NONE);
  }

  @Test
  public void testTextTransformAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-transform");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textTransform(
        zoom(
          interval(
            stop(2, textTransform(TEXT_TRANSFORM_NONE))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextTransform());
    assertNotNull(layer.getTextTransform().getFunction());
    assertEquals(CameraFunction.class, layer.getTextTransform().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getTextTransform().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getTextTransform().getFunction().getStops()).size());
  }

  @Test
  public void testTextTransformAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-transform");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textTransform(property("FeaturePropertyA", Stops.<String>identity()))
    );

    // Verify
    assertNotNull(layer.getTextTransform());
    assertNotNull(layer.getTextTransform().getFunction());
    assertEquals(SourceFunction.class, layer.getTextTransform().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextTransform().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getTextTransform().getFunction().getStops().getClass());
  }

  @Test
  public void testTextTransformAsIntervalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-transform");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textTransform(
        property(
          "FeaturePropertyA",
          interval(
            stop(1, textTransform(TEXT_TRANSFORM_NONE))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextTransform());
    assertNotNull(layer.getTextTransform().getFunction());
    assertEquals(SourceFunction.class, layer.getTextTransform().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextTransform().getFunction()).getProperty());
    assertEquals(IntervalStops.class, layer.getTextTransform().getFunction().getStops().getClass());
  }


  @Test
  public void testTextOffsetAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-offset");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textOffset(new Float[]{0f,0f}));
    assertEquals((Float[]) layer.getTextOffset().getValue(), (Float[]) new Float[]{0f,0f});
  }

  @Test
  public void testTextOffsetAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-offset");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textOffset(
        zoom(
          exponential(
            stop(2, textOffset(new Float[]{0f,0f}))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextOffset());
    assertNotNull(layer.getTextOffset().getFunction());
    assertEquals(CameraFunction.class, layer.getTextOffset().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getTextOffset().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getTextOffset().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getTextOffset().getFunction().getStops()).size());
  }

  @Test
  public void testTextOffsetAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-offset");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textOffset(property("FeaturePropertyA", Stops.<Float[]>identity()))
    );

    // Verify
    assertNotNull(layer.getTextOffset());
    assertNotNull(layer.getTextOffset().getFunction());
    assertEquals(SourceFunction.class, layer.getTextOffset().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextOffset().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getTextOffset().getFunction().getStops().getClass());
  }

  @Test
  public void testTextOffsetAsIntervalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-offset");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textOffset(
        property(
          "FeaturePropertyA",
          interval(
            stop(1, textOffset(new Float[]{0f,0f}))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextOffset());
    assertNotNull(layer.getTextOffset().getFunction());
    assertEquals(SourceFunction.class, layer.getTextOffset().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextOffset().getFunction()).getProperty());
    assertEquals(IntervalStops.class, layer.getTextOffset().getFunction().getStops().getClass());
  }


  @Test
  public void testTextAllowOverlapAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-allow-overlap");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textAllowOverlap(true));
    assertEquals((Boolean) layer.getTextAllowOverlap().getValue(), (Boolean) true);
  }

  @Test
  public void testTextAllowOverlapAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-allow-overlap");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textAllowOverlap(
        zoom(
          interval(
            stop(2, textAllowOverlap(true))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextAllowOverlap());
    assertNotNull(layer.getTextAllowOverlap().getFunction());
    assertEquals(CameraFunction.class, layer.getTextAllowOverlap().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getTextAllowOverlap().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getTextAllowOverlap().getFunction().getStops()).size());
  }


  @Test
  public void testTextIgnorePlacementAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-ignore-placement");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textIgnorePlacement(true));
    assertEquals((Boolean) layer.getTextIgnorePlacement().getValue(), (Boolean) true);
  }

  @Test
  public void testTextIgnorePlacementAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-ignore-placement");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textIgnorePlacement(
        zoom(
          interval(
            stop(2, textIgnorePlacement(true))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextIgnorePlacement());
    assertNotNull(layer.getTextIgnorePlacement().getFunction());
    assertEquals(CameraFunction.class, layer.getTextIgnorePlacement().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getTextIgnorePlacement().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getTextIgnorePlacement().getFunction().getStops()).size());
  }


  @Test
  public void testTextOptionalAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-optional");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textOptional(true));
    assertEquals((Boolean) layer.getTextOptional().getValue(), (Boolean) true);
  }

  @Test
  public void testTextOptionalAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-optional");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textOptional(
        zoom(
          interval(
            stop(2, textOptional(true))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextOptional());
    assertNotNull(layer.getTextOptional().getFunction());
    assertEquals(CameraFunction.class, layer.getTextOptional().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getTextOptional().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getTextOptional().getFunction().getStops()).size());
  }

  @Test
  public void testIconOpacityTransition() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-opacityTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setIconOpacityTransition(options);
    assertEquals(layer.getIconOpacityTransition(), options);
  }

  @Test
  public void testIconOpacityAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-opacity");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconOpacity(0.3f));
    assertEquals((Float) layer.getIconOpacity().getValue(), (Float) 0.3f);
  }

  @Test
  public void testIconOpacityAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconOpacity(
        zoom(
          exponential(
            stop(2, iconOpacity(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconOpacity());
    assertNotNull(layer.getIconOpacity().getFunction());
    assertEquals(CameraFunction.class, layer.getIconOpacity().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getIconOpacity().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getIconOpacity().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getIconOpacity().getFunction().getStops()).size());
  }

  @Test
  public void testIconOpacityAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconOpacity(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getIconOpacity());
    assertNotNull(layer.getIconOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getIconOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconOpacity().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getIconOpacity().getFunction().getStops().getClass());
  }

  @Test
  public void testIconOpacityAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconOpacity(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, iconOpacity(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconOpacity());
    assertNotNull(layer.getIconOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getIconOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconOpacity().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getIconOpacity().getFunction().getStops().getClass());
  }

  @Test
  public void testIconOpacityAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconOpacity(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, iconOpacity(0.3f))
          )
        ).withDefaultValue(iconOpacity(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getIconOpacity());
    assertNotNull(layer.getIconOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getIconOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconOpacity().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getIconOpacity().getFunction().getStops().getClass());
    assertNotNull(((SourceFunction) layer.getIconOpacity().getFunction()).getDefaultValue());
    assertNotNull(((SourceFunction) layer.getIconOpacity().getFunction()).getDefaultValue().getValue());
    assertEquals(0.3f, ((SourceFunction) layer.getIconOpacity().getFunction()).getDefaultValue().getValue());
  }

  @Test
  public void testIconOpacityAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconOpacity(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, iconOpacity(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(iconOpacity(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getIconOpacity());
    assertNotNull(layer.getIconOpacity().getFunction());
    assertEquals(CompositeFunction.class, layer.getIconOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getIconOpacity().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getIconOpacity().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getIconOpacity().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getIconOpacity().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testIconColorTransition() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-colorTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setIconColorTransition(options);
    assertEquals(layer.getIconColorTransition(), options);
  }

  @Test
  public void testIconColorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconColor("rgba(0, 0, 0, 1)"));
    assertEquals((String) layer.getIconColor().getValue(), (String) "rgba(0, 0, 0, 1)");
  }

  @Test
  public void testIconColorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconColor(
        zoom(
          exponential(
            stop(2, iconColor("rgba(0, 0, 0, 1)"))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconColor());
    assertNotNull(layer.getIconColor().getFunction());
    assertEquals(CameraFunction.class, layer.getIconColor().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getIconColor().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getIconColor().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getIconColor().getFunction().getStops()).size());
  }

  @Test
  public void testIconColorAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconColor(property("FeaturePropertyA", Stops.<String>identity()))
    );

    // Verify
    assertNotNull(layer.getIconColor());
    assertNotNull(layer.getIconColor().getFunction());
    assertEquals(SourceFunction.class, layer.getIconColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconColor().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getIconColor().getFunction().getStops().getClass());
  }

  @Test
  public void testIconColorAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconColor(
        property(
          "FeaturePropertyA",
          exponential(
            stop(Color.RED, iconColor(Color.RED))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconColor());
    assertNotNull(layer.getIconColor().getFunction());
    assertEquals(SourceFunction.class, layer.getIconColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconColor().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getIconColor().getFunction().getStops().getClass());
  }

  @Test
  public void testIconColorAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconColor(
        property(
          "FeaturePropertyA",
          categorical(
            stop("valueA", iconColor(Color.RED))
          )
        ).withDefaultValue(iconColor(Color.GREEN))
      )
    );

    // Verify
    assertNotNull(layer.getIconColor());
    assertNotNull(layer.getIconColor().getFunction());
    assertEquals(SourceFunction.class, layer.getIconColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconColor().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getIconColor().getFunction().getStops().getClass());
    assertNotNull(((SourceFunction) layer.getIconColor().getFunction()).getDefaultValue());
    assertNotNull(((SourceFunction) layer.getIconColor().getFunction()).getDefaultValue().getValue());
    assertEquals(Color.GREEN, (int) ((SourceFunction) layer.getIconColor().getFunction()).getDefaultValue().getColorInt());
  }

  @Test
  public void testIconColorAsIntConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconColor(Color.RED));
    assertEquals(layer.getIconColorAsInt(), Color.RED);
  }

  @Test
  public void testIconHaloColorTransition() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-colorTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setIconHaloColorTransition(options);
    assertEquals(layer.getIconHaloColorTransition(), options);
  }

  @Test
  public void testIconHaloColorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconHaloColor("rgba(0, 0, 0, 1)"));
    assertEquals((String) layer.getIconHaloColor().getValue(), (String) "rgba(0, 0, 0, 1)");
  }

  @Test
  public void testIconHaloColorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconHaloColor(
        zoom(
          exponential(
            stop(2, iconHaloColor("rgba(0, 0, 0, 1)"))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconHaloColor());
    assertNotNull(layer.getIconHaloColor().getFunction());
    assertEquals(CameraFunction.class, layer.getIconHaloColor().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getIconHaloColor().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getIconHaloColor().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getIconHaloColor().getFunction().getStops()).size());
  }

  @Test
  public void testIconHaloColorAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconHaloColor(property("FeaturePropertyA", Stops.<String>identity()))
    );

    // Verify
    assertNotNull(layer.getIconHaloColor());
    assertNotNull(layer.getIconHaloColor().getFunction());
    assertEquals(SourceFunction.class, layer.getIconHaloColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconHaloColor().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getIconHaloColor().getFunction().getStops().getClass());
  }

  @Test
  public void testIconHaloColorAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconHaloColor(
        property(
          "FeaturePropertyA",
          exponential(
            stop(Color.RED, iconHaloColor(Color.RED))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconHaloColor());
    assertNotNull(layer.getIconHaloColor().getFunction());
    assertEquals(SourceFunction.class, layer.getIconHaloColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconHaloColor().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getIconHaloColor().getFunction().getStops().getClass());
  }

  @Test
  public void testIconHaloColorAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconHaloColor(
        property(
          "FeaturePropertyA",
          categorical(
            stop("valueA", iconHaloColor(Color.RED))
          )
        ).withDefaultValue(iconHaloColor(Color.GREEN))
      )
    );

    // Verify
    assertNotNull(layer.getIconHaloColor());
    assertNotNull(layer.getIconHaloColor().getFunction());
    assertEquals(SourceFunction.class, layer.getIconHaloColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconHaloColor().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getIconHaloColor().getFunction().getStops().getClass());
    assertNotNull(((SourceFunction) layer.getIconHaloColor().getFunction()).getDefaultValue());
    assertNotNull(((SourceFunction) layer.getIconHaloColor().getFunction()).getDefaultValue().getValue());
    assertEquals(Color.GREEN, (int) ((SourceFunction) layer.getIconHaloColor().getFunction()).getDefaultValue().getColorInt());
  }

  @Test
  public void testIconHaloColorAsIntConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconHaloColor(Color.RED));
    assertEquals(layer.getIconHaloColorAsInt(), Color.RED);
  }

  @Test
  public void testIconHaloWidthTransition() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-widthTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setIconHaloWidthTransition(options);
    assertEquals(layer.getIconHaloWidthTransition(), options);
  }

  @Test
  public void testIconHaloWidthAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-width");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconHaloWidth(0.3f));
    assertEquals((Float) layer.getIconHaloWidth().getValue(), (Float) 0.3f);
  }

  @Test
  public void testIconHaloWidthAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconHaloWidth(
        zoom(
          exponential(
            stop(2, iconHaloWidth(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconHaloWidth());
    assertNotNull(layer.getIconHaloWidth().getFunction());
    assertEquals(CameraFunction.class, layer.getIconHaloWidth().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getIconHaloWidth().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getIconHaloWidth().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getIconHaloWidth().getFunction().getStops()).size());
  }

  @Test
  public void testIconHaloWidthAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconHaloWidth(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getIconHaloWidth());
    assertNotNull(layer.getIconHaloWidth().getFunction());
    assertEquals(SourceFunction.class, layer.getIconHaloWidth().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconHaloWidth().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getIconHaloWidth().getFunction().getStops().getClass());
  }

  @Test
  public void testIconHaloWidthAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconHaloWidth(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, iconHaloWidth(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconHaloWidth());
    assertNotNull(layer.getIconHaloWidth().getFunction());
    assertEquals(SourceFunction.class, layer.getIconHaloWidth().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconHaloWidth().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getIconHaloWidth().getFunction().getStops().getClass());
  }

  @Test
  public void testIconHaloWidthAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconHaloWidth(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, iconHaloWidth(0.3f))
          )
        ).withDefaultValue(iconHaloWidth(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getIconHaloWidth());
    assertNotNull(layer.getIconHaloWidth().getFunction());
    assertEquals(SourceFunction.class, layer.getIconHaloWidth().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconHaloWidth().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getIconHaloWidth().getFunction().getStops().getClass());
    assertNotNull(((SourceFunction) layer.getIconHaloWidth().getFunction()).getDefaultValue());
    assertNotNull(((SourceFunction) layer.getIconHaloWidth().getFunction()).getDefaultValue().getValue());
    assertEquals(0.3f, ((SourceFunction) layer.getIconHaloWidth().getFunction()).getDefaultValue().getValue());
  }

  @Test
  public void testIconHaloWidthAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconHaloWidth(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, iconHaloWidth(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(iconHaloWidth(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getIconHaloWidth());
    assertNotNull(layer.getIconHaloWidth().getFunction());
    assertEquals(CompositeFunction.class, layer.getIconHaloWidth().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getIconHaloWidth().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getIconHaloWidth().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getIconHaloWidth().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getIconHaloWidth().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testIconHaloBlurTransition() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-blurTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setIconHaloBlurTransition(options);
    assertEquals(layer.getIconHaloBlurTransition(), options);
  }

  @Test
  public void testIconHaloBlurAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-blur");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconHaloBlur(0.3f));
    assertEquals((Float) layer.getIconHaloBlur().getValue(), (Float) 0.3f);
  }

  @Test
  public void testIconHaloBlurAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconHaloBlur(
        zoom(
          exponential(
            stop(2, iconHaloBlur(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconHaloBlur());
    assertNotNull(layer.getIconHaloBlur().getFunction());
    assertEquals(CameraFunction.class, layer.getIconHaloBlur().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getIconHaloBlur().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getIconHaloBlur().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getIconHaloBlur().getFunction().getStops()).size());
  }

  @Test
  public void testIconHaloBlurAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconHaloBlur(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getIconHaloBlur());
    assertNotNull(layer.getIconHaloBlur().getFunction());
    assertEquals(SourceFunction.class, layer.getIconHaloBlur().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconHaloBlur().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getIconHaloBlur().getFunction().getStops().getClass());
  }

  @Test
  public void testIconHaloBlurAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconHaloBlur(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, iconHaloBlur(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconHaloBlur());
    assertNotNull(layer.getIconHaloBlur().getFunction());
    assertEquals(SourceFunction.class, layer.getIconHaloBlur().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconHaloBlur().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getIconHaloBlur().getFunction().getStops().getClass());
  }

  @Test
  public void testIconHaloBlurAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconHaloBlur(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, iconHaloBlur(0.3f))
          )
        ).withDefaultValue(iconHaloBlur(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getIconHaloBlur());
    assertNotNull(layer.getIconHaloBlur().getFunction());
    assertEquals(SourceFunction.class, layer.getIconHaloBlur().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getIconHaloBlur().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getIconHaloBlur().getFunction().getStops().getClass());
    assertNotNull(((SourceFunction) layer.getIconHaloBlur().getFunction()).getDefaultValue());
    assertNotNull(((SourceFunction) layer.getIconHaloBlur().getFunction()).getDefaultValue().getValue());
    assertEquals(0.3f, ((SourceFunction) layer.getIconHaloBlur().getFunction()).getDefaultValue().getValue());
  }

  @Test
  public void testIconHaloBlurAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-halo-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconHaloBlur(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, iconHaloBlur(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(iconHaloBlur(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getIconHaloBlur());
    assertNotNull(layer.getIconHaloBlur().getFunction());
    assertEquals(CompositeFunction.class, layer.getIconHaloBlur().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getIconHaloBlur().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getIconHaloBlur().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getIconHaloBlur().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getIconHaloBlur().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testIconTranslateTransition() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-translateTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setIconTranslateTransition(options);
    assertEquals(layer.getIconTranslateTransition(), options);
  }

  @Test
  public void testIconTranslateAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-translate");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconTranslate(new Float[]{0f,0f}));
    assertEquals((Float[]) layer.getIconTranslate().getValue(), (Float[]) new Float[]{0f,0f});
  }

  @Test
  public void testIconTranslateAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-translate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconTranslate(
        zoom(
          exponential(
            stop(2, iconTranslate(new Float[]{0f,0f}))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconTranslate());
    assertNotNull(layer.getIconTranslate().getFunction());
    assertEquals(CameraFunction.class, layer.getIconTranslate().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getIconTranslate().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getIconTranslate().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getIconTranslate().getFunction().getStops()).size());
  }


  @Test
  public void testIconTranslateAnchorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-translate-anchor");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(iconTranslateAnchor(ICON_TRANSLATE_ANCHOR_MAP));
    assertEquals((String) layer.getIconTranslateAnchor().getValue(), (String) ICON_TRANSLATE_ANCHOR_MAP);
  }

  @Test
  public void testIconTranslateAnchorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("icon-translate-anchor");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      iconTranslateAnchor(
        zoom(
          interval(
            stop(2, iconTranslateAnchor(ICON_TRANSLATE_ANCHOR_MAP))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getIconTranslateAnchor());
    assertNotNull(layer.getIconTranslateAnchor().getFunction());
    assertEquals(CameraFunction.class, layer.getIconTranslateAnchor().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getIconTranslateAnchor().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getIconTranslateAnchor().getFunction().getStops()).size());
  }

  @Test
  public void testTextOpacityTransition() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-opacityTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setTextOpacityTransition(options);
    assertEquals(layer.getTextOpacityTransition(), options);
  }

  @Test
  public void testTextOpacityAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-opacity");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textOpacity(0.3f));
    assertEquals((Float) layer.getTextOpacity().getValue(), (Float) 0.3f);
  }

  @Test
  public void testTextOpacityAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textOpacity(
        zoom(
          exponential(
            stop(2, textOpacity(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextOpacity());
    assertNotNull(layer.getTextOpacity().getFunction());
    assertEquals(CameraFunction.class, layer.getTextOpacity().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getTextOpacity().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getTextOpacity().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getTextOpacity().getFunction().getStops()).size());
  }

  @Test
  public void testTextOpacityAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textOpacity(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getTextOpacity());
    assertNotNull(layer.getTextOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getTextOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextOpacity().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getTextOpacity().getFunction().getStops().getClass());
  }

  @Test
  public void testTextOpacityAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textOpacity(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, textOpacity(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextOpacity());
    assertNotNull(layer.getTextOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getTextOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextOpacity().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getTextOpacity().getFunction().getStops().getClass());
  }

  @Test
  public void testTextOpacityAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textOpacity(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, textOpacity(0.3f))
          )
        ).withDefaultValue(textOpacity(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getTextOpacity());
    assertNotNull(layer.getTextOpacity().getFunction());
    assertEquals(SourceFunction.class, layer.getTextOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextOpacity().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getTextOpacity().getFunction().getStops().getClass());
    assertNotNull(((SourceFunction) layer.getTextOpacity().getFunction()).getDefaultValue());
    assertNotNull(((SourceFunction) layer.getTextOpacity().getFunction()).getDefaultValue().getValue());
    assertEquals(0.3f, ((SourceFunction) layer.getTextOpacity().getFunction()).getDefaultValue().getValue());
  }

  @Test
  public void testTextOpacityAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-opacity");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textOpacity(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, textOpacity(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(textOpacity(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getTextOpacity());
    assertNotNull(layer.getTextOpacity().getFunction());
    assertEquals(CompositeFunction.class, layer.getTextOpacity().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getTextOpacity().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getTextOpacity().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getTextOpacity().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getTextOpacity().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testTextColorTransition() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-colorTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setTextColorTransition(options);
    assertEquals(layer.getTextColorTransition(), options);
  }

  @Test
  public void testTextColorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textColor("rgba(0, 0, 0, 1)"));
    assertEquals((String) layer.getTextColor().getValue(), (String) "rgba(0, 0, 0, 1)");
  }

  @Test
  public void testTextColorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textColor(
        zoom(
          exponential(
            stop(2, textColor("rgba(0, 0, 0, 1)"))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextColor());
    assertNotNull(layer.getTextColor().getFunction());
    assertEquals(CameraFunction.class, layer.getTextColor().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getTextColor().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getTextColor().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getTextColor().getFunction().getStops()).size());
  }

  @Test
  public void testTextColorAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textColor(property("FeaturePropertyA", Stops.<String>identity()))
    );

    // Verify
    assertNotNull(layer.getTextColor());
    assertNotNull(layer.getTextColor().getFunction());
    assertEquals(SourceFunction.class, layer.getTextColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextColor().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getTextColor().getFunction().getStops().getClass());
  }

  @Test
  public void testTextColorAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textColor(
        property(
          "FeaturePropertyA",
          exponential(
            stop(Color.RED, textColor(Color.RED))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextColor());
    assertNotNull(layer.getTextColor().getFunction());
    assertEquals(SourceFunction.class, layer.getTextColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextColor().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getTextColor().getFunction().getStops().getClass());
  }

  @Test
  public void testTextColorAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textColor(
        property(
          "FeaturePropertyA",
          categorical(
            stop("valueA", textColor(Color.RED))
          )
        ).withDefaultValue(textColor(Color.GREEN))
      )
    );

    // Verify
    assertNotNull(layer.getTextColor());
    assertNotNull(layer.getTextColor().getFunction());
    assertEquals(SourceFunction.class, layer.getTextColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextColor().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getTextColor().getFunction().getStops().getClass());
    assertNotNull(((SourceFunction) layer.getTextColor().getFunction()).getDefaultValue());
    assertNotNull(((SourceFunction) layer.getTextColor().getFunction()).getDefaultValue().getValue());
    assertEquals(Color.GREEN, (int) ((SourceFunction) layer.getTextColor().getFunction()).getDefaultValue().getColorInt());
  }

  @Test
  public void testTextColorAsIntConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textColor(Color.RED));
    assertEquals(layer.getTextColorAsInt(), Color.RED);
  }

  @Test
  public void testTextHaloColorTransition() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-colorTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setTextHaloColorTransition(options);
    assertEquals(layer.getTextHaloColorTransition(), options);
  }

  @Test
  public void testTextHaloColorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textHaloColor("rgba(0, 0, 0, 1)"));
    assertEquals((String) layer.getTextHaloColor().getValue(), (String) "rgba(0, 0, 0, 1)");
  }

  @Test
  public void testTextHaloColorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textHaloColor(
        zoom(
          exponential(
            stop(2, textHaloColor("rgba(0, 0, 0, 1)"))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextHaloColor());
    assertNotNull(layer.getTextHaloColor().getFunction());
    assertEquals(CameraFunction.class, layer.getTextHaloColor().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getTextHaloColor().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getTextHaloColor().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getTextHaloColor().getFunction().getStops()).size());
  }

  @Test
  public void testTextHaloColorAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textHaloColor(property("FeaturePropertyA", Stops.<String>identity()))
    );

    // Verify
    assertNotNull(layer.getTextHaloColor());
    assertNotNull(layer.getTextHaloColor().getFunction());
    assertEquals(SourceFunction.class, layer.getTextHaloColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextHaloColor().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getTextHaloColor().getFunction().getStops().getClass());
  }

  @Test
  public void testTextHaloColorAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textHaloColor(
        property(
          "FeaturePropertyA",
          exponential(
            stop(Color.RED, textHaloColor(Color.RED))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextHaloColor());
    assertNotNull(layer.getTextHaloColor().getFunction());
    assertEquals(SourceFunction.class, layer.getTextHaloColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextHaloColor().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getTextHaloColor().getFunction().getStops().getClass());
  }

  @Test
  public void testTextHaloColorAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-color");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textHaloColor(
        property(
          "FeaturePropertyA",
          categorical(
            stop("valueA", textHaloColor(Color.RED))
          )
        ).withDefaultValue(textHaloColor(Color.GREEN))
      )
    );

    // Verify
    assertNotNull(layer.getTextHaloColor());
    assertNotNull(layer.getTextHaloColor().getFunction());
    assertEquals(SourceFunction.class, layer.getTextHaloColor().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextHaloColor().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getTextHaloColor().getFunction().getStops().getClass());
    assertNotNull(((SourceFunction) layer.getTextHaloColor().getFunction()).getDefaultValue());
    assertNotNull(((SourceFunction) layer.getTextHaloColor().getFunction()).getDefaultValue().getValue());
    assertEquals(Color.GREEN, (int) ((SourceFunction) layer.getTextHaloColor().getFunction()).getDefaultValue().getColorInt());
  }

  @Test
  public void testTextHaloColorAsIntConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textHaloColor(Color.RED));
    assertEquals(layer.getTextHaloColorAsInt(), Color.RED);
  }

  @Test
  public void testTextHaloWidthTransition() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-widthTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setTextHaloWidthTransition(options);
    assertEquals(layer.getTextHaloWidthTransition(), options);
  }

  @Test
  public void testTextHaloWidthAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-width");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textHaloWidth(0.3f));
    assertEquals((Float) layer.getTextHaloWidth().getValue(), (Float) 0.3f);
  }

  @Test
  public void testTextHaloWidthAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textHaloWidth(
        zoom(
          exponential(
            stop(2, textHaloWidth(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextHaloWidth());
    assertNotNull(layer.getTextHaloWidth().getFunction());
    assertEquals(CameraFunction.class, layer.getTextHaloWidth().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getTextHaloWidth().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getTextHaloWidth().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getTextHaloWidth().getFunction().getStops()).size());
  }

  @Test
  public void testTextHaloWidthAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textHaloWidth(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getTextHaloWidth());
    assertNotNull(layer.getTextHaloWidth().getFunction());
    assertEquals(SourceFunction.class, layer.getTextHaloWidth().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextHaloWidth().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getTextHaloWidth().getFunction().getStops().getClass());
  }

  @Test
  public void testTextHaloWidthAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textHaloWidth(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, textHaloWidth(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextHaloWidth());
    assertNotNull(layer.getTextHaloWidth().getFunction());
    assertEquals(SourceFunction.class, layer.getTextHaloWidth().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextHaloWidth().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getTextHaloWidth().getFunction().getStops().getClass());
  }

  @Test
  public void testTextHaloWidthAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textHaloWidth(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, textHaloWidth(0.3f))
          )
        ).withDefaultValue(textHaloWidth(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getTextHaloWidth());
    assertNotNull(layer.getTextHaloWidth().getFunction());
    assertEquals(SourceFunction.class, layer.getTextHaloWidth().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextHaloWidth().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getTextHaloWidth().getFunction().getStops().getClass());
    assertNotNull(((SourceFunction) layer.getTextHaloWidth().getFunction()).getDefaultValue());
    assertNotNull(((SourceFunction) layer.getTextHaloWidth().getFunction()).getDefaultValue().getValue());
    assertEquals(0.3f, ((SourceFunction) layer.getTextHaloWidth().getFunction()).getDefaultValue().getValue());
  }

  @Test
  public void testTextHaloWidthAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-width");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textHaloWidth(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, textHaloWidth(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(textHaloWidth(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getTextHaloWidth());
    assertNotNull(layer.getTextHaloWidth().getFunction());
    assertEquals(CompositeFunction.class, layer.getTextHaloWidth().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getTextHaloWidth().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getTextHaloWidth().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getTextHaloWidth().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getTextHaloWidth().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testTextHaloBlurTransition() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-blurTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setTextHaloBlurTransition(options);
    assertEquals(layer.getTextHaloBlurTransition(), options);
  }

  @Test
  public void testTextHaloBlurAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-blur");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textHaloBlur(0.3f));
    assertEquals((Float) layer.getTextHaloBlur().getValue(), (Float) 0.3f);
  }

  @Test
  public void testTextHaloBlurAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textHaloBlur(
        zoom(
          exponential(
            stop(2, textHaloBlur(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextHaloBlur());
    assertNotNull(layer.getTextHaloBlur().getFunction());
    assertEquals(CameraFunction.class, layer.getTextHaloBlur().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getTextHaloBlur().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getTextHaloBlur().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getTextHaloBlur().getFunction().getStops()).size());
  }

  @Test
  public void testTextHaloBlurAsIdentitySourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textHaloBlur(property("FeaturePropertyA", Stops.<Float>identity()))
    );

    // Verify
    assertNotNull(layer.getTextHaloBlur());
    assertNotNull(layer.getTextHaloBlur().getFunction());
    assertEquals(SourceFunction.class, layer.getTextHaloBlur().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextHaloBlur().getFunction()).getProperty());
    assertEquals(IdentityStops.class, layer.getTextHaloBlur().getFunction().getStops().getClass());
  }

  @Test
  public void testTextHaloBlurAsExponentialSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textHaloBlur(
        property(
          "FeaturePropertyA",
          exponential(
            stop(0.3f, textHaloBlur(0.3f))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextHaloBlur());
    assertNotNull(layer.getTextHaloBlur().getFunction());
    assertEquals(SourceFunction.class, layer.getTextHaloBlur().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextHaloBlur().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getTextHaloBlur().getFunction().getStops().getClass());
  }

  @Test
  public void testTextHaloBlurAsCategoricalSourceFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textHaloBlur(
        property(
          "FeaturePropertyA",
          categorical(
            stop(1.0f, textHaloBlur(0.3f))
          )
        ).withDefaultValue(textHaloBlur(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getTextHaloBlur());
    assertNotNull(layer.getTextHaloBlur().getFunction());
    assertEquals(SourceFunction.class, layer.getTextHaloBlur().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((SourceFunction) layer.getTextHaloBlur().getFunction()).getProperty());
    assertEquals(CategoricalStops.class, layer.getTextHaloBlur().getFunction().getStops().getClass());
    assertNotNull(((SourceFunction) layer.getTextHaloBlur().getFunction()).getDefaultValue());
    assertNotNull(((SourceFunction) layer.getTextHaloBlur().getFunction()).getDefaultValue().getValue());
    assertEquals(0.3f, ((SourceFunction) layer.getTextHaloBlur().getFunction()).getDefaultValue().getValue());
  }

  @Test
  public void testTextHaloBlurAsCompositeFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-halo-blur");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textHaloBlur(
        composite(
          "FeaturePropertyA",
          exponential(
            stop(0, 0.3f, textHaloBlur(0.9f))
          ).withBase(0.5f)
        ).withDefaultValue(textHaloBlur(0.3f))
      )
    );

    // Verify
    assertNotNull(layer.getTextHaloBlur());
    assertNotNull(layer.getTextHaloBlur().getFunction());
    assertEquals(CompositeFunction.class, layer.getTextHaloBlur().getFunction().getClass());
    assertEquals("FeaturePropertyA", ((CompositeFunction) layer.getTextHaloBlur().getFunction()).getProperty());
    assertEquals(ExponentialStops.class, layer.getTextHaloBlur().getFunction().getStops().getClass());
    assertEquals(1, ((ExponentialStops) layer.getTextHaloBlur().getFunction().getStops()).size());

    ExponentialStops<Stop.CompositeValue<Float, Float>, Float> stops =
      (ExponentialStops<Stop.CompositeValue<Float, Float>, Float>) layer.getTextHaloBlur().getFunction().getStops();
    Stop<Stop.CompositeValue<Float, Float>, Float> stop = stops.iterator().next();
    assertEquals(0f, stop.in.zoom, 0.001);
    assertEquals(0.3f, stop.in.value, 0.001f);
    assertEquals(0.9f, stop.out, 0.001f);
  }

  @Test
  public void testTextTranslateTransition() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-translateTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setTextTranslateTransition(options);
    assertEquals(layer.getTextTranslateTransition(), options);
  }

  @Test
  public void testTextTranslateAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-translate");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textTranslate(new Float[]{0f,0f}));
    assertEquals((Float[]) layer.getTextTranslate().getValue(), (Float[]) new Float[]{0f,0f});
  }

  @Test
  public void testTextTranslateAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-translate");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textTranslate(
        zoom(
          exponential(
            stop(2, textTranslate(new Float[]{0f,0f}))
          ).withBase(0.5f)
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextTranslate());
    assertNotNull(layer.getTextTranslate().getFunction());
    assertEquals(CameraFunction.class, layer.getTextTranslate().getFunction().getClass());
    assertEquals(ExponentialStops.class, layer.getTextTranslate().getFunction().getStops().getClass());
    assertEquals(0.5f, ((ExponentialStops) layer.getTextTranslate().getFunction().getStops()).getBase(), 0.001);
    assertEquals(1, ((ExponentialStops) layer.getTextTranslate().getFunction().getStops()).size());
  }


  @Test
  public void testTextTranslateAnchorAsConstant() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-translate-anchor");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(textTranslateAnchor(TEXT_TRANSLATE_ANCHOR_MAP));
    assertEquals((String) layer.getTextTranslateAnchor().getValue(), (String) TEXT_TRANSLATE_ANCHOR_MAP);
  }

  @Test
  public void testTextTranslateAnchorAsCameraFunction() {
    checkViewIsDisplayed(R.id.mapView);
    Timber.i("text-translate-anchor");
    assertNotNull(layer);

    // Set
    layer.setProperties(
      textTranslateAnchor(
        zoom(
          interval(
            stop(2, textTranslateAnchor(TEXT_TRANSLATE_ANCHOR_MAP))
          )
        )
      )
    );

    // Verify
    assertNotNull(layer.getTextTranslateAnchor());
    assertNotNull(layer.getTextTranslateAnchor().getFunction());
    assertEquals(CameraFunction.class, layer.getTextTranslateAnchor().getFunction().getClass());
    assertEquals(IntervalStops.class, layer.getTextTranslateAnchor().getFunction().getStops().getClass());
    assertEquals(1, ((IntervalStops) layer.getTextTranslateAnchor().getFunction().getStops()).size());
  }


  @After
  public void unregisterIntentServiceIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }
}
