package com.mapbox.mapboxgl.testapp;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxgl.geometry.LatLng;
import com.mapbox.mapboxgl.views.MapView;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class MapFragment extends Fragment {

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "MapFragment";

    //
    // Instance members
    //

    // The map
    private MapView mMap;

    //
    // Lifecycle events
    //

    // Called when the fragment is created
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        super.onCreateView(inflater, container, savedInstanceState);
        Log.v(TAG, "onCreateView");

        // Create the map
        mMap = (MapView) inflater.inflate(R.layout.fragment_main, container, true);

        // Load the access token
/*
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(getActivity().getApplicationContext().getResources().openRawResource(R.raw.token)));
            String line = reader.readLine();
            mMap.setAccessToken(line);
        } catch (IOException e) {
            return null;
        }
*/

        // Need to pass on any saved state to the map
        mMap.onCreate(savedInstanceState);

        // Return the map as the root view
        return mMap;
    }

    // Called when the fragment is destroyed
    @Override
    public void onDestroyView() {
        super.onDestroyView();
        Log.v(TAG, "onDestroyView");

        // Need to pass on to view
        mMap.onDestroy();
        mMap = null;
    }

    // Called when the fragment is visible
    @Override
    public void onStart() {
        super.onStart();
        Log.v(TAG, "onStart");

        // Need to pass on to view
        mMap.onStart();
        mMap.setCenterCoordinate(new LatLng(38.247887,-121.843872));
        mMap.setZoomLevel(7);
    }

    // Called when the fragment is invisible
    @Override
    public void onStop() {
        super.onStop();
        Log.v(TAG, "onStop");

        // Need to pass on to view
        mMap.onStop();
    }

    // Called when the fragment is in the background
    @Override
    public void onPause() {
        super.onPause();
        Log.v(TAG, "onPause");

        // Need to pass on to view
        mMap.onPause();
    }

    // Called when the fragment is no longer in the background
    @Override
    public void onResume() {
        super.onResume();
        Log.v(TAG, "onResume");

        // Need to pass on to view
        mMap.onResume();
    }

    // Called before fragment is destroyed
    @Override
    public void onSaveInstanceState(Bundle outState) {
        Log.v(TAG, "onSaveInstanceState");

        // Need to retrieve any saved state from the map
        mMap.onSaveInstanceState(outState);
        super.onSaveInstanceState(outState);
    }

    //
    // Property methods
    //

    public MapView getMap() {
        return mMap;
    }
}
