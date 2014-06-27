package com.mapbox.mapboxgl.lib;

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

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "MapFragment";

    // The map
    private MapView mMapView;

    // The style attrs to load into the map
    AttributeSet mAttrs;

    //
    // Lifecycle events
    //

    // Called when the fragment is created
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        super.onCreateView(inflater, container, savedInstanceState);
        Log.v(TAG, "onCreateView");

        // Create the map
        mMapView = new MapView(getActivity());

        // Load the attributes
        TypedArray typedArray = getActivity().obtainStyledAttributes(mAttrs,
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

        // Need to pass on any saved state to the map
        mMapView.onCreate(savedInstanceState);

        // Return the map as the root view
        return mMapView;
    }

    // Called when the fragment is destroyed
    @Override
    public void onDestroyView() {
        super.onDestroyView();
        Log.v(TAG, "onDestroyView");

        // Need to pass on to view
        mMapView.onDestroy();
        mMapView = null;
    }

    // Called to load any fragment attributes set in the activity's layout
    @Override
    public void onInflate(Activity activity, AttributeSet attrs,
            Bundle savedInstanceState) {
        super.onInflate(activity, attrs, savedInstanceState);
        Log.v(TAG, "onInflate");

        // TODO seems this is called before onCreateView
        // Kinda strange, is it supposed to be like this?
        // Need to keep a copy of these and pass on to MapView later

        mAttrs = attrs;
    }

    // Called when the fragment is visible
    @Override
    public void onStart() {
        super.onStart();
        Log.v(TAG, "onStart");

        // Need to pass on to view
        mMapView.onStart();
    }

    // Called when the fragment is invisible
    @Override
    public void onStop() {
        super.onStop();
        Log.v(TAG, "onStop");

        // Need to pass on to view
        mMapView.onStop();
    }

    // Called when the fragment is in the background
    @Override
    public void onPause() {
        super.onPause();
        Log.v(TAG, "onPause");

        // Need to pass on to view
        mMapView.onPause();
    }

    // Called when the fragment is no longer in the background
    @Override
    public void onResume() {
        super.onResume();
        Log.v(TAG, "onResume");

        // Need to pass on to view
        mMapView.onResume();
    }

    // Called before fragment is destroyed
    @Override
    public void onSaveInstanceState(Bundle outState) {
        Log.v(TAG, "onSaveInstanceState");

        // Need to retrieve any saved state from the map
        mMapView.onSaveInstanceState(outState);
        super.onSaveInstanceState(outState);
    }
}
