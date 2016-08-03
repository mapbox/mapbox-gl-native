package com.mapbox.mapboxsdk.activity;

import android.app.Activity;
import android.support.test.espresso.Espresso;
import android.util.Log;

import com.mapbox.mapboxsdk.activity.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;

import org.junit.After;
import org.junit.Before;

public abstract class BaseMapboxMapTest extends BaseTest {

    private OnMapReadyIdlingResource idlingResource;
    protected MapboxMap mapboxMap;

    @Before
    public void registerIdlingResource() {
        Log.e(MapboxConstants.TAG, "@Before test");
        idlingResource = new OnMapReadyIdlingResource(getActivity());
        Espresso.registerIdlingResources(idlingResource);
        checkViewIsDisplayed(R.id.mapView);
        mapboxMap = idlingResource.getMapboxMap();
    }

    public abstract Activity getActivity();

    @After
    public void unregisterIdlingResource() {
        Log.e(MapboxConstants.TAG, "@After test");
        Espresso.unregisterIdlingResources(idlingResource);
    }
}
