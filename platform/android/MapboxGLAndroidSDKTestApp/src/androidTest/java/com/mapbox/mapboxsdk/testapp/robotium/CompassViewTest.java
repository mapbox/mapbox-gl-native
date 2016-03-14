package com.mapbox.mapboxsdk.testapp.robotium;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.widgets.CompassView;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.robotium.utils.ViewAssertion;

import org.junit.Test;

public class CompassViewTest extends BaseMainActivityTest {

    private CompassView compassView;

    @Override
    public void setUp() throws Exception {
        super.setUp();
        compassView = (CompassView) solo.getView(R.id.compassView);
    }

    @Test
    public void testCompass() {
        setCameraDirection(0);
        solo.sleep(200);
        ViewAssertion.isInvisible(compassView);
    }

    @Test
    public void testCompassClick() throws Exception {
        setCameraDirection(20);
        solo.sleep(200);
        ViewAssertion.isVisible(compassView);
        solo.clickOnView(compassView);
        solo.sleep(200);
        ViewAssertion.isVisible(compassView);
    }

    private void setCameraDirection(float bearing) {
        mapboxMap.setCameraPosition(new CameraPosition.Builder()
                .target(new LatLng(0, 0))
                .tilt(0)
                .zoom(15)
                .bearing(bearing)
                .build());
    }
}