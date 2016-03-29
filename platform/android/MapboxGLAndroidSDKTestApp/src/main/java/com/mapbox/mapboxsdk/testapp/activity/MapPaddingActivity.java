package com.mapbox.mapboxsdk.testapp.activity;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.TrackingSettings;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.ApiAccess;

public class MapPaddingActivity extends AppCompatActivity {

    private MapView mMapView;
    private MapboxMap mMapboxMap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map_padding);

        final Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.setTag(true);
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.onCreate(savedInstanceState);

        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                mMapboxMap = mapboxMap;

                int paddingLeft = (int) getResources().getDimension(R.dimen.map_padding_left);
                int paddingBottom = (int) getResources().getDimension(R.dimen.map_padding_bottom);
                int paddingRight = (int) getResources().getDimension(R.dimen.map_padding_right);
                mapboxMap.setPadding(paddingLeft, toolbar.getHeight(), paddingRight, paddingBottom);

                moveToBangalore();
            }
        });
    }

    @Override
    public void onResume() {
        super.onResume();
        mMapView.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        mMapView.onPause();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mMapView.onSaveInstanceState(outState);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mMapView.onLowMemory();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_padding, menu);
        return true;
    }

    private void toggleGps(boolean enable) {
        try {
            // Enable user location
            mMapboxMap.setMyLocationEnabled(enable);

            TrackingSettings trackingSettings = mMapboxMap.getTrackingSettings();
            trackingSettings.setDismissTrackingOnGesture(false);
            trackingSettings.setMyLocationTrackingMode(enable ? MyLocationTracking.TRACKING_FOLLOW : MyLocationTracking.TRACKING_NONE);
        } catch (SecurityException e) {
            // permission not granted is handled in FeatureOverviewActivity
            finish();
        }
    }

    private void moveToBangalore() {
        toggleGps(false);

        LatLng bangalore = new LatLng(12.9810816, 77.6368034);
        CameraPosition cameraPosition = new CameraPosition.Builder()
                .zoom(16)
                .target(bangalore)
                .bearing(40)
                .tilt(45)
                .build();

        mMapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
        mMapboxMap.addMarker(new MarkerOptions().title("Center map").position(bangalore));
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;

            case R.id.action_user_tracking:
                if (mMapboxMap != null) {
                    toggleGps(true);
                }
                return true;

            case R.id.action_bangalore:
                if (mMapboxMap != null) {
                    moveToBangalore();
                }
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }

}

