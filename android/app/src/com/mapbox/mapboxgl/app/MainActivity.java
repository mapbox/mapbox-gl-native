package com.mapbox.mapboxgl.app;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import com.mapbox.mapboxgl.lib.MapView;

public class MainActivity extends Activity {

    // TODO add comments

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "MainActivity";

    private MapView mMapView;

    //
    // Lifecycle events
    //

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "onCreate");
        setContentView(R.layout.activity_main);
        mMapView = (MapView) findViewById(R.id.map);

        mMapView.onCreate(savedInstanceState);
    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.v(TAG, "onStart");
        mMapView.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.v(TAG, "onStop");
        mMapView.onStop();
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.v(TAG, "onPause");
        mMapView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.v(TAG, "onResume");
        mMapView.onResume();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        Log.v(TAG, "onSaveInstanceState");
        mMapView.onSaveInstanceState(outState);
        super.onSaveInstanceState(outState);
    }
}
