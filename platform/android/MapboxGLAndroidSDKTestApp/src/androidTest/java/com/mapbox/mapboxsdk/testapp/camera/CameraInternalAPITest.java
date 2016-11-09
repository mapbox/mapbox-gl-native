package com.mapbox.mapboxsdk.testapp.camera;

import android.support.test.espresso.Espresso;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.rule.ActivityTestRule;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapViewUtils;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.testapp.utils.TestConstants;
import com.mapbox.mapboxsdk.testapp.utils.ViewUtils;

import org.hamcrest.Matcher;
import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static org.junit.Assert.assertEquals;

/**
 * Tests camera transformations that aren't part of our public API
 */
public class CameraInternalAPITest {

    @Rule
    public final ActivityTestRule<EspressoTestActivity> rule = new ActivityTestRule<>(EspressoTestActivity.class);

    private OnMapReadyIdlingResource idlingResource;

    @Before
    public void registerIdlingResource() {
        idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
        Espresso.registerIdlingResources(idlingResource);
    }

    @Test
    public void testBearing() {
        ViewUtils.checkViewIsDisplayed(R.id.mapView);
        EspressoTestActivity activity = rule.getActivity();
        MapboxMap mapboxMap = activity.getMapboxMap();

        CameraPosition initialPosition = new CameraPosition.Builder().target(new LatLng()).zoom(1).bearing(0).tilt(0).build();
        CameraPosition cameraPosition = mapboxMap.getCameraPosition();
        assertEquals("Default camera position should match default", cameraPosition, initialPosition);

        MapView mapView = (MapView) activity.findViewById(R.id.mapView);
        onView(withId(R.id.mapView)).perform(new BearingAction(mapView));
        assertEquals("Bearing should match", 45.1f, MapViewUtils.getDirection(mapView), TestConstants.BEARING_DELTA);
    }

    @Test
    public void testTilt() {
        ViewUtils.checkViewIsDisplayed(R.id.mapView);
        EspressoTestActivity activity = rule.getActivity();
        MapboxMap mapboxMap = activity.getMapboxMap();

        CameraPosition initialPosition = new CameraPosition.Builder().target(new LatLng()).zoom(1).bearing(0).tilt(0).build();
        CameraPosition cameraPosition = mapboxMap.getCameraPosition();
        assertEquals("Default camera position should match default", cameraPosition, initialPosition);

        MapView mapView = (MapView) activity.findViewById(R.id.mapView);
        onView(withId(R.id.mapView)).perform(new TiltAction(mapView));
        assertEquals("Tilt should match", 40.0f, MapViewUtils.getTilt(mapView), TestConstants.TILT_DELTA);
    }

    @Test
    public void testLatLng() {
        ViewUtils.checkViewIsDisplayed(R.id.mapView);
        EspressoTestActivity activity = rule.getActivity();
        MapboxMap mapboxMap = activity.getMapboxMap();

        CameraPosition initialPosition = new CameraPosition.Builder().target(new LatLng()).zoom(1).bearing(0).tilt(0).build();
        CameraPosition cameraPosition = mapboxMap.getCameraPosition();
        assertEquals("Default camera position should match default", cameraPosition, initialPosition);

        MapView mapView = (MapView) activity.findViewById(R.id.mapView);
        onView(withId(R.id.mapView)).perform(new LatLngAction(mapView));

        LatLng centerCoordinate = MapViewUtils.getLatLng(mapView);
        assertEquals("Latitude should match", 1.1f, centerCoordinate.getLatitude(), TestConstants.LAT_LNG_DELTA);
        assertEquals("Longitude should match", 2.2f, centerCoordinate.getLongitude(), TestConstants.LAT_LNG_DELTA);
    }

    @After
    public void unregisterIdlingResource() {
        Espresso.unregisterIdlingResources(idlingResource);
    }

    private class BearingAction implements ViewAction {

        private MapView mapView;

        BearingAction(MapView mapView) {
            this.mapView = mapView;
        }

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
            MapViewUtils.setDirection(mapView, -45.1f);
        }
    }

    private class TiltAction implements ViewAction {

        private MapView mapView;

        TiltAction(MapView mapView) {
            this.mapView = mapView;
        }

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
            MapViewUtils.setTilt(mapView, 40.0f);
        }
    }

    private class LatLngAction implements ViewAction {

        private MapView mapView;

        LatLngAction(MapView mapView) {
            this.mapView = mapView;
        }

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
            MapViewUtils.setLatLng(mapView, new LatLng(1.1, 2.2));
        }
    }
}
