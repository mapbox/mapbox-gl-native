package com.mapbox.mapboxsdk.style;

import android.support.test.espresso.Espresso;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.layers.NoSuchLayerException;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.style.sources.VectorSource;
import com.mapbox.mapboxsdk.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

/**
 * Basic smoke tests for Layer and Source
 */
@RunWith(AndroidJUnit4.class)
public class RuntimeStyleTests extends BaseTest {

    @Rule
    public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

    private OnMapReadyIdlingResource idlingResource;

    @Before
    public void registerIdlingResource() {
        idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
        Espresso.registerIdlingResources(idlingResource);
    }

    @Test
    public void testGetAddRemoveLayer() {
        checkViewIsDisplayed(R.id.mapView);

        MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

        //Get initial
        assertNotNull(mapboxMap.getLayer("building"));

        //Remove
        try {
            mapboxMap.removeLayer("building");
        } catch (NoSuchLayerException e) {
            assertFalse(true);
        }
        assertNull(mapboxMap.getLayer("building"));

        //Add
        FillLayer layer = new FillLayer("building", "composite");
        layer.setSourceLayer("building");
        mapboxMap.addLayer(layer);

        assertNotNull(mapboxMap.getLayer("building"));

        try {
            layer.setProperties(PropertyFactory.visibility(Property.VISIBLE));
            assertTrue("Never reached as the reference is invalid after adding", false);
        } catch (Exception e) {
            //Expected, reference is no longer valid
        }
    }

    @Test
    public void testAddRemoveSource() {
        checkViewIsDisplayed(R.id.mapView);

        MapboxMap mapboxMap = rule.getActivity().getMapboxMap();
        mapboxMap.addSource(new VectorSource("my-source", "mapbox://mapbox.mapbox-terrain-v2"));
        mapboxMap.removeSource("my-source");
    }

    @After
    public void unregisterIntentServiceIdlingResource() {
        Espresso.unregisterIdlingResources(idlingResource);
    }
}
