package com.mapbox.mapboxsdk.camera;

import android.app.Activity;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.rule.ActivityTestRule;
import android.view.View;

import com.mapbox.mapboxsdk.activity.BaseMapboxMapTest;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.maplayout.DebugModeActivity;

import org.hamcrest.Matcher;
import org.junit.Rule;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

public class ZoomTest extends BaseMapboxMapTest {

    @Rule
    public final ActivityTestRule<DebugModeActivity> rule = new ActivityTestRule<>(DebugModeActivity.class);

    @Test
    public void testZoom() throws Exception {
        onView(withId(R.id.mapView)).perform(new ZoomAction(MapboxConstants.MINIMUM_ZOOM, MapboxConstants.MAXIMUM_ZOOM));
    }

    @Override
    public Activity getActivity() {
        return rule.getActivity();
    }

    private class ZoomAction implements ViewAction {

        private float fromZoom;
        private float toZoom;

        public ZoomAction(float fromZoom, float toZoom) {
            this.fromZoom = fromZoom;
            this.toZoom = toZoom;
        }

        @Override
        public Matcher<View> getConstraints() {
            return isDisplayed();
        }

        @Override
        public String getDescription() {
            return "zoomAction";
        }

        @Override
        public void perform(UiController uiController, View view) {
            // move camera above denver
            mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(
                    new CameraPosition.Builder()
                            .target(new LatLng(39.749750, -104.949559))
                            .build()));

            uiController.loopMainThreadForAtLeast(500);

            for (float i = fromZoom; i < toZoom; i++) {
                mapboxMap.animateCamera(CameraUpdateFactory.zoomTo(i));
                uiController.loopMainThreadForAtLeast(200);
            }
        }
    }
}
