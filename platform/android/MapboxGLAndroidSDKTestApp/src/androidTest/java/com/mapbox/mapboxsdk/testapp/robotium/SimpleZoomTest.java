package com.mapbox.mapboxsdk.testapp.robotium;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;

import org.junit.Test;

public class SimpleZoomTest extends BaseMainActivityTest {

    @Override
    public void setUp() throws Exception {
        super.setUp();
    }

    @Test
    public void testZoomIn() {
        float currentZoomlevel = MapboxConstants.MINIMUM_ZOOM;
        while (currentZoomlevel < MapboxConstants.MAXIMUM_ZOOM - 5) {
            mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(47.376271, 8.539116),currentZoomlevel));
            currentZoomlevel++;
            solo.sleep(1400);
        }
    }

}
