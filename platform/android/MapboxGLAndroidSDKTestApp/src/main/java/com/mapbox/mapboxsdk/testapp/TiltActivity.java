package com.mapbox.mapboxsdk.testapp;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.maps.MapView;

public class TiltActivity extends AppCompatActivity {

    private final static LatLng WASHINGTON_DC = new LatLng(38.90252, -77.02291);

    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_tilt);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        // Set up the map
        mMapView = (MapView) findViewById(R.id.tiltMapView);
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {

                // Move camera to Washington DC
                CameraPosition normalCameraPosition = new CameraPosition.Builder()
                        .target(WASHINGTON_DC)
                        .zoom(11)
                        .build();
                mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(normalCameraPosition));

                // Animate camera tilting
                CameraPosition tiltedCameraPosition = new CameraPosition.Builder()
                        .target(WASHINGTON_DC)
                        .zoom(11)
                        .tilt(45.0f)
                        .build();
                mapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(tiltedCameraPosition), 10000);
            }
        });
    }

    @Override
    protected void onStart() {
        super.onStart();
        mMapView.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
    }

    @Override
    public void onPause() {
        super.onPause();
        mMapView.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();
        mMapView.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mMapView.onSaveInstanceState(outState);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

}
