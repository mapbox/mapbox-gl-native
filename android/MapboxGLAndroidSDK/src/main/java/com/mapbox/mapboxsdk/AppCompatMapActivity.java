package com.mapbox.mapboxsdk;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;

public class AppCompatMapActivity extends AppCompatActivity {

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "MapActivity";

    //
    // Instance members
    //

    // The access token
    private String mAccessToken;

    // The map style
    private String mStyleUrl;

    // The map
    private MapView mMap;

    //
    // Constructors
    //

    public AppCompatMapActivity(@Nullable String accessToken) {
        if (accessToken == null) {
            accessToken = ApiAccess.getToken(this);
        }

        mAccessToken = accessToken;
    }

    public AppCompatMapActivity(@Nullable String accessToken, @NonNull String styleUrl) {
        this(accessToken);
        mStyleUrl = styleUrl;
    }

    //
    // Lifecycle events
    //

    // Called when activity is created
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Create the map
        setContentView(R.layout.fragment_mapview);
        mMap = (MapView) findViewById(R.id.mapView);
        mMap.setAccessToken(mAccessToken);
        mMap.setStyleUrl(mStyleUrl);

        // Need to pass on any saved state to the map
        mMap.onCreate(savedInstanceState);
    }

    @Override
    protected void onStart() {
        super.onStart();
        mMap.onStart();
    }

    @Override
    public void onResume() {
        super.onResume();
        mMap.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        mMap.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mMap.onStop();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mMap.onSaveInstanceState(outState);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMap.onDestroy();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mMap.onLowMemory();
    }

    //
    // Property methods
    //

    @Nullable
    public MapView getMap() {
        return mMap;
    }
}
