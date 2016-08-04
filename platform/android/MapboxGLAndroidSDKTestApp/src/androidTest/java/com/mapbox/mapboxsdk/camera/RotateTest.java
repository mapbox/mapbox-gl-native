package com.mapbox.mapboxsdk.camera;

import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.view.View;

import com.mapbox.mapboxsdk.activity.BaseTest;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMapUtils;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.infowindow.InfoWindowActivity;

import org.hamcrest.Matcher;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

public class RotateTest extends BaseTest {

    @Test
    public void testRotate() {
        onView(withId(R.id.mapView)).perform(new RotateAction(0, 180));
    }

    @Override
    public Class getActivityClass() {
        return InfoWindowActivity.class;
    }

    private class RotateAction implements ViewAction {

        private float startDegree;
        private float endDegree;

        public RotateAction(float startDegree, float endDegree) {
            this.startDegree = startDegree;
            this.endDegree = endDegree;
        }

        @Override
        public Matcher<View> getConstraints() {
            return isDisplayed();
        }

        @Override
        public String getDescription() {
            return "rotateAction";
        }

        @Override
        public void perform(UiController uiController, View view) {
            uiController.loopMainThreadForAtLeast(500);
            for (float i = startDegree; i < endDegree; i++) {
                MapboxMapUtils.setDirection((MapView) view, i);
                uiController.loopMainThreadForAtLeast(1);
            }
        }
    }
}
