// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style;

import android.support.test.espresso.Espresso;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.util.Log;

import com.mapbox.mapboxsdk.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.RasterLayer;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import static com.mapbox.mapboxsdk.style.layers.Property.NONE;
import static com.mapbox.mapboxsdk.style.layers.Property.VISIBLE;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.rasterBrightnessMax;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.rasterBrightnessMin;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.rasterContrast;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.rasterFadeDuration;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.rasterHueRotate;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.rasterOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.rasterSaturation;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.visibility;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 * Basic smoke tests for RasterLayer
 */
@RunWith(AndroidJUnit4.class)
public class RasterLayerTest extends BaseTest {
    private static final String TAG = RasterLayerTest.class.getSimpleName();

    @Rule
    public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

    private RasterLayer layer;

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
            layer = new RasterLayer("my-layer", "composite");
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
    public void testRasterOpacity() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new RasterLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "raster-opacity");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(rasterOpacity(0.3f));
        assertEquals((Float) layer.getRasterOpacity().getValue(), (Float) 0.3f);
    }

    @Test
    public void testRasterHueRotate() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new RasterLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "raster-hue-rotate");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(rasterHueRotate(0.3f));
        assertEquals((Float) layer.getRasterHueRotate().getValue(), (Float) 0.3f);
    }

    @Test
    public void testRasterBrightnessMin() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new RasterLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "raster-brightness-min");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(rasterBrightnessMin(0.3f));
        assertEquals((Float) layer.getRasterBrightnessMin().getValue(), (Float) 0.3f);
    }

    @Test
    public void testRasterBrightnessMax() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new RasterLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "raster-brightness-max");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(rasterBrightnessMax(0.3f));
        assertEquals((Float) layer.getRasterBrightnessMax().getValue(), (Float) 0.3f);
    }

    @Test
    public void testRasterSaturation() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new RasterLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "raster-saturation");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(rasterSaturation(0.3f));
        assertEquals((Float) layer.getRasterSaturation().getValue(), (Float) 0.3f);
    }

    @Test
    public void testRasterContrast() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new RasterLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "raster-contrast");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(rasterContrast(0.3f));
        assertEquals((Float) layer.getRasterContrast().getValue(), (Float) 0.3f);
    }

    @Test
    public void testRasterFadeDuration() {
        checkViewIsDisplayed(R.id.mapView);

        mapboxMap = rule.getActivity().getMapboxMap();

        if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
            Log.i(TAG, "Adding layer");
            layer = new RasterLayer("my-layer", "composite");
            layer.setSourceLayer("composite");
            mapboxMap.addLayer(layer);
            //Layer reference is now stale, get new reference
            layer = mapboxMap.getLayerAs("my-layer");
        }
        Log.i(TAG, "raster-fade-duration");
        assertNotNull(layer);

        //Set and Get
        layer.setProperties(rasterFadeDuration(0.3f));
        assertEquals((Float) layer.getRasterFadeDuration().getValue(), (Float) 0.3f);
    }


    @After
    public void unregisterIntentServiceIdlingResource() {
        Espresso.unregisterIdlingResources(idlingResource);
    }
}
