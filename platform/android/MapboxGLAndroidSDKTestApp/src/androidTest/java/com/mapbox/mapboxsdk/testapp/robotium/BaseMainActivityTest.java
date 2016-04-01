package com.mapbox.mapboxsdk.testapp.robotium;

import android.test.ActivityInstrumentationTestCase2;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapUtils;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.annotation.PolylineActivity;
import com.robotium.solo.Solo;

import org.junit.Test;

public abstract class BaseMainActivityTest extends ActivityInstrumentationTestCase2<PolylineActivity> {

    protected Solo solo;
    protected MapView mapView;
    protected MapboxMap mapboxMap;

    public BaseMainActivityTest() {
        super(PolylineActivity.class);
    }

    @Override
    public void setUp() throws Exception {
        solo = new Solo(getInstrumentation(), getActivity());
        mapView = (MapView) solo.getView(R.id.mapView);
        mapboxMap = MapboxMapUtils.getMapboxMap(mapView);
    }

    @Override
    public void tearDown() throws Exception {
        solo.finishOpenedActivities();
    }

    @Test
    public void testSanity() throws Exception {
        assertNotNull("Should be non null", solo);
        assertNotNull("Should be non null", mapView);
        assertNotNull("Should be non null", mapboxMap);
    }
}
