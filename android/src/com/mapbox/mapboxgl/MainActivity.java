package com.mapbox.mapboxgl;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import com.mapbox.mapboxgl.MapView;

public class MainActivity extends Activity {

    private static final String TAG = "MainActivity";

    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "onCreate");
        setContentView(R.layout.activity_main);
        mMapView = (MapView) findViewById(R.id.map);
        
        // TODO wrapper fragment?
		mMapView.onCreate(savedInstanceState);
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
