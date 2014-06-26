package com.mapbox.mapboxgl;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class FragmentActivity extends Activity {

    // TODO add comments

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "FragmentActivity";

    //
    // Lifecycle events
    //

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "onCreate");
        setContentView(R.layout.activity_fragment);
    }
}
