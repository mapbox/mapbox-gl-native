package com.mapbox.mapboxgl.app;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import com.mapbox.mapboxgl.lib.MapView;

public class MainActivity extends Activity {

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "MainActivity";

    // The map
    private MapView mMapView;

    //
    // Lifecycle events
    //

    // Called when the activity is created
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "onCreate");

        // Load the layout
        setContentView(R.layout.activity_main);
        mMapView = (MapView) findViewById(R.id.map);

        // Need to pass on any saved state to the map
        mMapView.onCreate(savedInstanceState);
    }

    // Called when the activity is destroyed
    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.v(TAG, "onDestroy");

        // Need to pass on to view
        mMapView.onDestroy();
    }

    // Called when the activity is visible
    @Override
    protected void onStart() {
        super.onStart();
        Log.v(TAG, "onStart");

        // Need to pass on to view
        mMapView.onStart();
    }

    // Called when the activity is invisible
    @Override
    protected void onStop() {
        super.onStop();
        Log.v(TAG, "onStop");

        // Need to pass on to view
        mMapView.onStop();
    }

    // Called when the activity is in the background
    @Override
    protected void onPause() {
        super.onPause();
        Log.v(TAG, "onPause");

        // Need to pass on to view
        mMapView.onPause();
    }

    // Called when the activity is no longer in the background
    @Override
    protected void onResume() {
        super.onResume();
        Log.v(TAG, "onResume");

        // Need to pass on to view
        mMapView.onResume();
    }

    // Called before activity is destroyed
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        Log.v(TAG, "onSaveInstanceState");

        // Need to retrieve any saved state from the map
        mMapView.onSaveInstanceState(outState);
        super.onSaveInstanceState(outState);
    }
}
