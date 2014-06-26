package com.mapbox.mapboxgl.app;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class FragmentActivity extends Activity {

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "FragmentActivity";

    //
    // Lifecycle events
    //

    // Called when activity is created
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "onCreate");

        // Load the layout
        setContentView(R.layout.activity_fragment);
    }
}
