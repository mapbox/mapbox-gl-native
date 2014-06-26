package com.mapbox.mapboxgl;

import android.app.Activity;
import android.app.Fragment;
import android.content.res.TypedArray;
import android.os.Bundle;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

public class MapFragment extends Fragment {

    // TODO add comments

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "MapFragment";

    private MapView mMapView;

    //
    // Lifecycle events
    //

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        super.onCreateView(inflater, container, savedInstanceState);
        Log.v(TAG, "onCreateView");

        mMapView = new MapView(getActivity());
        mMapView.onCreate(savedInstanceState);

        return mMapView;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        mMapView = null;
    }

    @Override
    public void onInflate(Activity activity, AttributeSet attrs,
            Bundle savedInstanceState) {
        super.onInflate(activity, attrs, savedInstanceState);
        Log.v(TAG, "onInflate");

        // Load attributes
        TypedArray typedArray = activity.obtainStyledAttributes(attrs,
                R.styleable.MapFragment, 0, 0);

        try {
            double centerLongitude = typedArray.getFloat(
                    R.styleable.MapFragment_centerLongitude, 0.0f);
            double centerLatitude = typedArray.getFloat(
                    R.styleable.MapFragment_centerLatitude, 0.0f);
            LonLat centerCoordinate = new LonLat(centerLongitude,
                    centerLatitude);
            mMapView.setCenterCoordinate(centerCoordinate);
            mMapView.setDirection(typedArray.getFloat(
                    R.styleable.MapFragment_direction, 0.0f));
            mMapView.setZoomLevel(typedArray.getFloat(
                    R.styleable.MapFragment_zoomLevel, 0.0f));
            mMapView.setZoomEnabled(typedArray.getBoolean(
                    R.styleable.MapFragment_zoomEnabled, true));
            mMapView.setScrollEnabled(typedArray.getBoolean(
                    R.styleable.MapFragment_scrollEnabled, true));
            mMapView.setRotateEnabled(typedArray.getBoolean(
                    R.styleable.MapFragment_rotateEnabled, true));
            mMapView.setDebugActive(typedArray.getBoolean(
                    R.styleable.MapFragment_debugActive, false));
        } finally {
            typedArray.recycle();
        }
    }

    // TODO crashes because lifecycle events are not being called
    @Override
    public void onStart() {
        super.onStart();
        Log.v(TAG, "onStart");
        mMapView.onStart();
    }

    @Override
    public void onStop() {
        super.onStop();
        Log.v(TAG, "onStop");
        mMapView.onStop();
    }

    @Override
    public void onPause() {
        super.onPause();
        Log.v(TAG, "onPause");
        mMapView.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.v(TAG, "onResume");
        mMapView.onResume();
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        Log.v(TAG, "onSaveInstanceState");
        mMapView.onSaveInstanceState(outState);
        super.onSaveInstanceState(outState);
    }
}
