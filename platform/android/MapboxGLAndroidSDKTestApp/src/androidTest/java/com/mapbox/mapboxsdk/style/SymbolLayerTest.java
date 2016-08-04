// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style;

import android.graphics.Color;
import android.support.test.espresso.Espresso;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.util.Log;

import com.mapbox.mapboxsdk.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import static com.mapbox.mapboxsdk.style.layers.Property.ICON_ROTATION_ALIGNMENT_MAP;
import static com.mapbox.mapboxsdk.style.layers.Property.ICON_TEXT_FIT_NONE;
import static com.mapbox.mapboxsdk.style.layers.Property.ICON_TRANSLATE_ANCHOR_MAP;
import static com.mapbox.mapboxsdk.style.layers.Property.NONE;
import static com.mapbox.mapboxsdk.style.layers.Property.SYMBOL_PLACEMENT_POINT;
import static com.mapbox.mapboxsdk.style.layers.Property.TEXT_ANCHOR_CENTER;
import static com.mapbox.mapboxsdk.style.layers.Property.TEXT_JUSTIFY_LEFT;
import static com.mapbox.mapboxsdk.style.layers.Property.TEXT_PITCH_ALIGNMENT_MAP;
import static com.mapbox.mapboxsdk.style.layers.Property.TEXT_ROTATION_ALIGNMENT_MAP;
import static com.mapbox.mapboxsdk.style.layers.Property.TEXT_TRANSFORM_NONE;
import static com.mapbox.mapboxsdk.style.layers.Property.TEXT_TRANSLATE_ANCHOR_MAP;
import static com.mapbox.mapboxsdk.style.layers.Property.VISIBLE;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconHaloBlur;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconHaloColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconHaloWidth;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconIgnorePlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconKeepUpright;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconOffset;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconOptional;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconPadding;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconRotate;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconRotationAlignment;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconSize;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconTextFit;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconTextFitPadding;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconTranslate;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconTranslateAnchor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.symbolAvoidEdges;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.symbolPlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.symbolSpacing;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textAnchor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textField;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textFont;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textHaloBlur;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textHaloColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textHaloWidth;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textIgnorePlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textJustify;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textKeepUpright;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textLetterSpacing;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textLineHeight;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textMaxAngle;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textMaxWidth;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textOffset;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textOptional;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textPadding;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textPitchAlignment;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textRotate;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textRotationAlignment;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textSize;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textTransform;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textTranslate;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textTranslateAnchor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.visibility;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 * Basic smoke tests for SymbolLayer
 */
@RunWith(AndroidJUnit4.class)
public class SymbolLayerTest extends BaseTest {
    private static final String TAG = SymbolLayerTest.class.getSimpleName();

    @Rule
    public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

    private SymbolLayer layer;

    private OnMapReadyIdlingResource idlingResource;

    private MapboxMap mapboxMap;

    @Before
    public void setup() {
        idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
        Espresso.registerIdlingResources(idlingResource);
    }

    @Test
    public void testSetVisibility() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "visibility");
        assertNotNull(layer);

        //Get initial
        assertEquals(layer.getVisibility().getValue(), VISIBLE);

        //Set
        layer.setProperties(visibility(NONE));
        assertEquals(layer.getVisibility().getValue(), NONE);
    }

    @Test
    public void testSymbolPlacement() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "symbol-placement");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(symbolPlacement(SYMBOL_PLACEMENT_POINT));
        assertEquals((String) layer.getSymbolPlacement().getValue(), (String) SYMBOL_PLACEMENT_POINT);
    }

    @Test
    public void testSymbolSpacing() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "symbol-spacing");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(symbolSpacing(0.3f));
        assertEquals((Float) layer.getSymbolSpacing().getValue(), (Float) 0.3f);
    }

    @Test
    public void testSymbolAvoidEdges() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "symbol-avoid-edges");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(symbolAvoidEdges(true));
        assertEquals((Boolean) layer.getSymbolAvoidEdges().getValue(), (Boolean) true);
    }

    @Test
    public void testIconAllowOverlap() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-allow-overlap");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconAllowOverlap(true));
        assertEquals((Boolean) layer.getIconAllowOverlap().getValue(), (Boolean) true);
    }

    @Test
    public void testIconIgnorePlacement() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-ignore-placement");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconIgnorePlacement(true));
        assertEquals((Boolean) layer.getIconIgnorePlacement().getValue(), (Boolean) true);
    }

    @Test
    public void testIconOptional() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-optional");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconOptional(true));
        assertEquals((Boolean) layer.getIconOptional().getValue(), (Boolean) true);
    }

    @Test
    public void testIconRotationAlignment() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-rotation-alignment");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconRotationAlignment(ICON_ROTATION_ALIGNMENT_MAP));
        assertEquals((String) layer.getIconRotationAlignment().getValue(), (String) ICON_ROTATION_ALIGNMENT_MAP);
    }

    @Test
    public void testIconSize() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-size");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconSize(0.3f));
        assertEquals((Float) layer.getIconSize().getValue(), (Float) 0.3f);
    }

    @Test
    public void testIconTextFit() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-text-fit");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconTextFit(ICON_TEXT_FIT_NONE));
        assertEquals((String) layer.getIconTextFit().getValue(), (String) ICON_TEXT_FIT_NONE);
    }

    @Test
    public void testIconTextFitPadding() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-text-fit-padding");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconTextFitPadding(new Float[]{0f, 0f, 0f, 0f}));
        assertEquals((Float[]) layer.getIconTextFitPadding().getValue(), (Float[]) new Float[]{0f, 0f, 0f, 0f});
    }

    @Test
    public void testIconImage() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-image");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconImage("undefined"));
        assertEquals((String) layer.getIconImage().getValue(), (String) "undefined");
    }

    @Test
    public void testIconRotate() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-rotate");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconRotate(0.3f));
        assertEquals((Float) layer.getIconRotate().getValue(), (Float) 0.3f);
    }

    @Test
    public void testIconPadding() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-padding");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconPadding(0.3f));
        assertEquals((Float) layer.getIconPadding().getValue(), (Float) 0.3f);
    }

    @Test
    public void testIconKeepUpright() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-keep-upright");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconKeepUpright(true));
        assertEquals((Boolean) layer.getIconKeepUpright().getValue(), (Boolean) true);
    }

    @Test
    public void testIconOffset() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-offset");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconOffset(new Float[]{0f, 0f}));
        assertEquals((Float[]) layer.getIconOffset().getValue(), (Float[]) new Float[]{0f, 0f});
    }

    @Test
    public void testTextPitchAlignment() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-pitch-alignment");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textPitchAlignment(TEXT_PITCH_ALIGNMENT_MAP));
        assertEquals((String) layer.getTextPitchAlignment().getValue(), (String) TEXT_PITCH_ALIGNMENT_MAP);
    }

    @Test
    public void testTextRotationAlignment() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-rotation-alignment");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textRotationAlignment(TEXT_ROTATION_ALIGNMENT_MAP));
        assertEquals((String) layer.getTextRotationAlignment().getValue(), (String) TEXT_ROTATION_ALIGNMENT_MAP);
    }

    @Test
    public void testTextField() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-field");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textField(""));
        assertEquals((String) layer.getTextField().getValue(), (String) "");
    }

    @Test
    public void testTextFont() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-font");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textFont(new String[]{"Open Sans Regular", "Arial Unicode MS Regular"}));
        assertEquals((String[]) layer.getTextFont().getValue(), (String[]) new String[]{"Open Sans Regular", "Arial Unicode MS Regular"});
    }

    @Test
    public void testTextSize() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-size");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textSize(0.3f));
        assertEquals((Float) layer.getTextSize().getValue(), (Float) 0.3f);
    }

    @Test
    public void testTextMaxWidth() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-max-width");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textMaxWidth(0.3f));
        assertEquals((Float) layer.getTextMaxWidth().getValue(), (Float) 0.3f);
    }

    @Test
    public void testTextLineHeight() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-line-height");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textLineHeight(0.3f));
        assertEquals((Float) layer.getTextLineHeight().getValue(), (Float) 0.3f);
    }

    @Test
    public void testTextLetterSpacing() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-letter-spacing");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textLetterSpacing(0.3f));
        assertEquals((Float) layer.getTextLetterSpacing().getValue(), (Float) 0.3f);
    }

    @Test
    public void testTextJustify() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-justify");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textJustify(TEXT_JUSTIFY_LEFT));
        assertEquals((String) layer.getTextJustify().getValue(), (String) TEXT_JUSTIFY_LEFT);
    }

    @Test
    public void testTextAnchor() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-anchor");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textAnchor(TEXT_ANCHOR_CENTER));
        assertEquals((String) layer.getTextAnchor().getValue(), (String) TEXT_ANCHOR_CENTER);
    }

    @Test
    public void testTextMaxAngle() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-max-angle");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textMaxAngle(0.3f));
        assertEquals((Float) layer.getTextMaxAngle().getValue(), (Float) 0.3f);
    }

    @Test
    public void testTextRotate() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-rotate");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textRotate(0.3f));
        assertEquals((Float) layer.getTextRotate().getValue(), (Float) 0.3f);
    }

    @Test
    public void testTextPadding() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-padding");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textPadding(0.3f));
        assertEquals((Float) layer.getTextPadding().getValue(), (Float) 0.3f);
    }

    @Test
    public void testTextKeepUpright() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-keep-upright");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textKeepUpright(true));
        assertEquals((Boolean) layer.getTextKeepUpright().getValue(), (Boolean) true);
    }

    @Test
    public void testTextTransform() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-transform");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textTransform(TEXT_TRANSFORM_NONE));
        assertEquals((String) layer.getTextTransform().getValue(), (String) TEXT_TRANSFORM_NONE);
    }

    @Test
    public void testTextOffset() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-offset");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textOffset(new Float[]{0f, 0f}));
        assertEquals((Float[]) layer.getTextOffset().getValue(), (Float[]) new Float[]{0f, 0f});
    }

    @Test
    public void testTextAllowOverlap() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-allow-overlap");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textAllowOverlap(true));
        assertEquals((Boolean) layer.getTextAllowOverlap().getValue(), (Boolean) true);
    }

    @Test
    public void testTextIgnorePlacement() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-ignore-placement");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textIgnorePlacement(true));
        assertEquals((Boolean) layer.getTextIgnorePlacement().getValue(), (Boolean) true);
    }

    @Test
    public void testTextOptional() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-optional");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textOptional(true));
        assertEquals((Boolean) layer.getTextOptional().getValue(), (Boolean) true);
    }

    @Test
    public void testIconOpacity() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-opacity");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconOpacity(0.3f));
        assertEquals((Float) layer.getIconOpacity().getValue(), (Float) 0.3f);
    }

    @Test
    public void testIconColor() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-color");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconColor("rgba(0, 0, 0, 1)"));
        assertEquals((String) layer.getIconColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    }

    @Test
    public void testIconColorAsInt() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-color");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconColor(Color.RED));
        assertEquals(layer.getIconColorAsInt(), Color.RED);
    }

    @Test
    public void testIconHaloColor() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-halo-color");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconHaloColor("rgba(0, 0, 0, 1)"));
        assertEquals((String) layer.getIconHaloColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    }

    @Test
    public void testIconHaloColorAsInt() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-halo-color");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconHaloColor(Color.RED));
        assertEquals(layer.getIconHaloColorAsInt(), Color.RED);
    }

    @Test
    public void testIconHaloWidth() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-halo-width");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconHaloWidth(0.3f));
        assertEquals((Float) layer.getIconHaloWidth().getValue(), (Float) 0.3f);
    }

    @Test
    public void testIconHaloBlur() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-halo-blur");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconHaloBlur(0.3f));
        assertEquals((Float) layer.getIconHaloBlur().getValue(), (Float) 0.3f);
    }

    @Test
    public void testIconTranslate() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-translate");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconTranslate(new Float[]{0f, 0f}));
        assertEquals((Float[]) layer.getIconTranslate().getValue(), (Float[]) new Float[]{0f, 0f});
    }

    @Test
    public void testIconTranslateAnchor() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "icon-translate-anchor");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(iconTranslateAnchor(ICON_TRANSLATE_ANCHOR_MAP));
        assertEquals((String) layer.getIconTranslateAnchor().getValue(), (String) ICON_TRANSLATE_ANCHOR_MAP);
    }

    @Test
    public void testTextOpacity() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-opacity");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textOpacity(0.3f));
        assertEquals((Float) layer.getTextOpacity().getValue(), (Float) 0.3f);
    }

    @Test
    public void testTextColor() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-color");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textColor("rgba(0, 0, 0, 1)"));
        assertEquals((String) layer.getTextColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    }

    @Test
    public void testTextColorAsInt() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-color");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textColor(Color.RED));
        assertEquals(layer.getTextColorAsInt(), Color.RED);
    }

    @Test
    public void testTextHaloColor() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-halo-color");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textHaloColor("rgba(0, 0, 0, 1)"));
        assertEquals((String) layer.getTextHaloColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    }

    @Test
    public void testTextHaloColorAsInt() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-halo-color");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textHaloColor(Color.RED));
        assertEquals(layer.getTextHaloColorAsInt(), Color.RED);
    }

    @Test
    public void testTextHaloWidth() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-halo-width");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textHaloWidth(0.3f));
        assertEquals((Float) layer.getTextHaloWidth().getValue(), (Float) 0.3f);
    }

    @Test
    public void testTextHaloBlur() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-halo-blur");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textHaloBlur(0.3f));
        assertEquals((Float) layer.getTextHaloBlur().getValue(), (Float) 0.3f);
    }

    @Test
    public void testTextTranslate() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-translate");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textTranslate(new Float[]{0f, 0f}));
        assertEquals((Float[]) layer.getTextTranslate().getValue(), (Float[]) new Float[]{0f, 0f});
    }

    @Test
    public void testTextTranslateAnchor() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new SymbolLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "text-translate-anchor");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(textTranslateAnchor(TEXT_TRANSLATE_ANCHOR_MAP));
        assertEquals((String) layer.getTextTranslateAnchor().getValue(), (String) TEXT_TRANSLATE_ANCHOR_MAP);
    }


    @After
    public void unregisterIntentServiceIdlingResource() {
        Espresso.unregisterIdlingResources(idlingResource);
    }
}
