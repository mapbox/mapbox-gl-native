package com.mapbox.mapboxsdk.testapp.style;

import android.support.test.espresso.Espresso;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;
import android.view.View;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.layers.NoSuchLayerException;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.style.sources.NoSuchSourceException;
import com.mapbox.mapboxsdk.style.sources.VectorSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.testapp.utils.ViewUtils;

import org.hamcrest.Matcher;
import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

/**
 * Basic smoke tests for Layer and Source
 */
@RunWith(AndroidJUnit4.class)
public class RuntimeStyleTests {

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
        ViewUtils.checkViewIsDisplayed(R.id.mapView);
        onView(withId(R.id.mapView)).perform(new AddRemoveLayerAction());
    }

    @Test
    public void testAddRemoveSource() {
        ViewUtils.checkViewIsDisplayed(R.id.mapView);

        MapboxMap mapboxMap = rule.getActivity().getMapboxMap();
        mapboxMap.addSource(new VectorSource("my-source", "mapbox://mapbox.mapbox-terrain-v2"));
        try {
            mapboxMap.removeSource("my-source");
        } catch (NoSuchSourceException e) {
            // it's ok..
        }
    }

    private class AddRemoveLayerAction implements ViewAction {

        @Override
        public Matcher<View> getConstraints() {
            return isDisplayed();
        }

        @Override
        public String getDescription() {
            return getClass().getSimpleName();
        }

        @Override
        public void perform(UiController uiController, View view) {
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
    }

    @After
    public void unregisterIntentServiceIdlingResource() {
        Espresso.unregisterIdlingResources(idlingResource);
    }
}
