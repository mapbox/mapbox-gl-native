package com.mapbox.mapboxgl.app;

import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;

import com.mapbox.mapboxgl.lib.MapView;

public class MainActivity extends ActionBarActivity {

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "MainActivity";

    //
    // Lifecycle events
    //

    // Called when activity is created
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "onCreate");

        // Show the app icon
        //getActionBar().setHomeButtonEnabled(true);

        // Load the layout
        setContentView(R.layout.activity_main);
    }
}
