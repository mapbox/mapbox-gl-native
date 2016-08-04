package com.mapbox.mapboxsdk.utils;

import android.app.Activity;
import android.support.test.espresso.IdlingResource;
import android.util.Log;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.maps.MapboxMap;

import java.lang.reflect.Field;

public class OnMapReadyIdlingResource implements IdlingResource {

    private final Activity activity;
    private MapboxMap mapboxMap;
    private IdlingResource.ResourceCallback resourceCallback;

    public OnMapReadyIdlingResource(Activity activity) {
        this.activity = activity;
    }

    @Override
    public String getName() {
        return getClass().getSimpleName();
    }

    @Override
    public boolean isIdleNow() {
        boolean idle = isMapboxMapReady();
        if (idle && resourceCallback != null) {
            resourceCallback.onTransitionToIdle();
        }
        return idle;
    }

    @Override
    public void registerIdleTransitionCallback(ResourceCallback resourceCallback) {
        this.resourceCallback = resourceCallback;
    }

    private boolean isMapboxMapReady() {
        try {
            Field field = activity.getClass().getDeclaredField("mapboxMap");
            field.setAccessible(true);
            mapboxMap = (MapboxMap) field.get(activity);
            Log.e(MapboxConstants.TAG, "isMapboxReady called with value " + (mapboxMap != null));
            return mapboxMap != null;
        } catch (Exception e) {
            Log.e(MapboxConstants.TAG, "could not reflect", e);
            return false;
        }
    }

    public MapboxMap getMapboxMap() {
        return mapboxMap;
    }
}
