package com.mapbox.mapboxgl.app;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxgl.lib.LonLat;
import com.mapbox.mapboxgl.lib.MapView;

public class MapFragment extends Fragment {

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "MapFragment";

    // The map
    private MapView mMapView;

    //
    // Lifecycle events
    //

    // Called when the fragment is created
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        super.onCreateView(inflater, container, savedInstanceState);
        Log.v(TAG, "onCreateView");

        // Create the map
        mMapView = (MapView) inflater.inflate(R.layout.fragment_main, container, true);

        // Load the API key
        mMapView.setAccessToken("pk.eyJ1IjoibGpiYWRlIiwiYSI6IlJSQ0FEZ2MifQ.7mE4aOegldh3595AG9dxpQ");

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

    //
    // Property methods
    //

    public MapView getMapView() {
        return mMapView;
    }
}
