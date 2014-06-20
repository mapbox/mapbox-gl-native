package com.mapbox.mapboxgl;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import com.mapbox.mapboxgl.MapView;

public class MainActivity extends Activity {

    private static String TAG = "MainActivity";

    private MapView map;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG, "onCreate");
        setContentView(R.layout.activity_main);
        map = (MapView) findViewById(R.id.map);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.i(TAG, "onDestroy");
    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.i(TAG, "onStart");
        // TODO
    }

    @Override
    protected void onRestart() {
        super.onRestart();
        Log.i(TAG, "onRestart");
        // TODO
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.i(TAG, "onStop");
        map.onStop();
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.i(TAG, "onPause");
        map.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.i(TAG, "onResume");
        map.onResume();
    }
}
