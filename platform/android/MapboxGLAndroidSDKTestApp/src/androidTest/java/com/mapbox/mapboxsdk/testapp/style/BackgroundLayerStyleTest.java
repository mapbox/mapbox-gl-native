// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.support.test.espresso.Espresso;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.util.Log;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.BackgroundLayer;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.*;
import static com.mapbox.mapboxsdk.style.layers.Property.*;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.*;

/**
 * Basic smoke tests for BackgroundLayer
 */
@RunWith(AndroidJUnit4.class)
public class BackgroundLayerStyleTest extends BaseStyleTest {
    private static final String TAG = BackgroundLayerStyleTest.class.getSimpleName();

    @Rule
    public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

    private BackgroundLayer layer;

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

        Log.i(TAG, "Retrieving layer");
        layer = mapboxMap.getLayerAs("background");
        Log.i(TAG, "visibility");
        assertNotNull(layer);

        //Get initial
        assertEquals(layer.getVisibility().getValue(), VISIBLE);

        //Set
        layer.setProperties(visibility(NONE));
        assertEquals(layer.getVisibility().getValue(), NONE);
    }

    @Test
    public void testBackgroundColor() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        Log.i(TAG, "Retrieving layer");
        layer = mapboxMap.getLayerAs("background");
        Log.i(TAG, "background-color");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(backgroundColor("rgba(0, 0, 0, 1)"));
        assertEquals((String) layer.getBackgroundColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    }

    @Test
    public void testBackgroundColorAsInt() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        Log.i(TAG, "Retrieving layer");
        layer = mapboxMap.getLayerAs("background");
        Log.i(TAG, "background-color");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(backgroundColor(Color.RED));
        assertEquals(layer.getBackgroundColorAsInt(), Color.RED);
    }

    @Test
    public void testBackgroundPattern() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        Log.i(TAG, "Retrieving layer");
        layer = mapboxMap.getLayerAs("background");
        Log.i(TAG, "background-pattern");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(backgroundPattern("pedestrian-polygon"));
        assertEquals((String) layer.getBackgroundPattern().getValue(), (String) "pedestrian-polygon");
    }

    @Test
    public void testBackgroundOpacity() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        Log.i(TAG, "Retrieving layer");
        layer = mapboxMap.getLayerAs("background");
        Log.i(TAG, "background-opacity");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(backgroundOpacity(0.3f));
        assertEquals((Float) layer.getBackgroundOpacity().getValue(), (Float) 0.3f);
    }


   @After
   public void unregisterIntentServiceIdlingResource() {
       Espresso.unregisterIdlingResources(idlingResource);
   }
}
