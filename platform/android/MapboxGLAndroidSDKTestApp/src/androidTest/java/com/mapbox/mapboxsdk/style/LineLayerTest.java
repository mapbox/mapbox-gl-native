// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style;

import android.graphics.Color;
import android.support.test.espresso.Espresso;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.util.Log;

import com.mapbox.mapboxsdk.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.LineLayer;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import static com.mapbox.mapboxsdk.style.layers.Property.LINE_CAP_BUTT;
import static com.mapbox.mapboxsdk.style.layers.Property.LINE_JOIN_BEVEL;
import static com.mapbox.mapboxsdk.style.layers.Property.LINE_TRANSLATE_ANCHOR_MAP;
import static com.mapbox.mapboxsdk.style.layers.Property.NONE;
import static com.mapbox.mapboxsdk.style.layers.Property.VISIBLE;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineBlur;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineCap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineDasharray;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineGapWidth;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineJoin;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineMiterLimit;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineOffset;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.linePattern;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineRoundLimit;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineTranslate;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineTranslateAnchor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineWidth;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.visibility;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 * Basic smoke tests for LineLayer
 */
@RunWith(AndroidJUnit4.class)
public class LineLayerTest extends BaseTest {
    private static final String TAG = LineLayerTest.class.getSimpleName();

    @Rule
    public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

    private LineLayer layer;

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
            layer = new LineLayer("my-layer", "composite");
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
    public void testLineCap() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-cap");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(lineCap(LINE_CAP_BUTT));
        assertEquals((String) layer.getLineCap().getValue(), (String) LINE_CAP_BUTT);
    }

    @Test
    public void testLineJoin() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-join");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(lineJoin(LINE_JOIN_BEVEL));
        assertEquals((String) layer.getLineJoin().getValue(), (String) LINE_JOIN_BEVEL);
    }

    @Test
    public void testLineMiterLimit() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-miter-limit");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(lineMiterLimit(0.3f));
        assertEquals((Float) layer.getLineMiterLimit().getValue(), (Float) 0.3f);
    }

    @Test
    public void testLineRoundLimit() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-round-limit");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(lineRoundLimit(0.3f));
        assertEquals((Float) layer.getLineRoundLimit().getValue(), (Float) 0.3f);
    }

    @Test
    public void testLineOpacity() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-opacity");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(lineOpacity(0.3f));
        assertEquals((Float) layer.getLineOpacity().getValue(), (Float) 0.3f);
    }

    @Test
    public void testLineColor() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-color");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(lineColor("rgba(0, 0, 0, 1)"));
        assertEquals((String) layer.getLineColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    }

    @Test
    public void testLineColorAsInt() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-color");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(lineColor(Color.RED));
        assertEquals(layer.getLineColorAsInt(), Color.RED);
    }

    @Test
    public void testLineTranslate() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-translate");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(lineTranslate(new Float[]{0f, 0f}));
        assertEquals((Float[]) layer.getLineTranslate().getValue(), (Float[]) new Float[]{0f, 0f});
    }

    @Test
    public void testLineTranslateAnchor() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-translate-anchor");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(lineTranslateAnchor(LINE_TRANSLATE_ANCHOR_MAP));
        assertEquals((String) layer.getLineTranslateAnchor().getValue(), (String) LINE_TRANSLATE_ANCHOR_MAP);
    }

    @Test
    public void testLineWidth() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-width");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(lineWidth(0.3f));
        assertEquals((Float) layer.getLineWidth().getValue(), (Float) 0.3f);
    }

    @Test
    public void testLineGapWidth() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-gap-width");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(lineGapWidth(0.3f));
        assertEquals((Float) layer.getLineGapWidth().getValue(), (Float) 0.3f);
    }

    @Test
    public void testLineOffset() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-offset");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(lineOffset(0.3f));
        assertEquals((Float) layer.getLineOffset().getValue(), (Float) 0.3f);
    }

    @Test
    public void testLineBlur() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-blur");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(lineBlur(0.3f));
        assertEquals((Float) layer.getLineBlur().getValue(), (Float) 0.3f);
    }

    @Test
    public void testLineDasharray() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-dasharray");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(lineDasharray(new Float[]{}));
        assertEquals((Float[]) layer.getLineDasharray().getValue(), (Float[]) new Float[]{});
    }

    @Test
    public void testLinePattern() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new LineLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "line-pattern");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(linePattern("pedestrian-polygon"));
        assertEquals((String) layer.getLinePattern().getValue(), (String) "pedestrian-polygon");
    }


    @After
    public void unregisterIntentServiceIdlingResource() {
        Espresso.unregisterIdlingResources(idlingResource);
    }
}
