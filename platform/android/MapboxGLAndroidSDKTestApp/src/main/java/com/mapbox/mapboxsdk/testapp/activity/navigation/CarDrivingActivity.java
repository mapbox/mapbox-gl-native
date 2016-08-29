package com.mapbox.mapboxsdk.testapp.activity.navigation;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.res.ResourcesCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;


import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.TrackingSettings;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationViewSettings;
import com.mapbox.mapboxsdk.testapp.R;

public class CarDrivingActivity extends AppCompatActivity implements MapboxMap.OnMyLocationChangeListener {

    private static final int PERMISSIONS_LOCATION = 0;

    private MapView mMapView;
    private MapboxMap mapboxMap;
    private Location location;

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_car_driving);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                CarDrivingActivity.this.mapboxMap = mapboxMap;

                // view settings
                MyLocationViewSettings settings = mapboxMap.getMyLocationViewSettings();
                settings.setPadding(0, getWindow().getDecorView().getMeasuredHeight() / 3, 0, 0);

                // get car
                Drawable car = ResourcesCompat.getDrawable(getResources(), R.drawable.ic_taxi_top_small, getTheme());
                settings.setForegroundTintColor(Color.TRANSPARENT);
                settings.setForegroundDrawable(car, car);

                // remove accuracy circle
                settings.setAccuracyAlpha(0);

                // disable dismissal when a gesture occurs
                TrackingSettings trackingSettings = mapboxMap.getTrackingSettings();
                trackingSettings.setDismissLocationTrackingOnGesture(false);
                trackingSettings.setDismissBearingTrackingOnGesture(false);

                mapboxMap.setOnMyLocationChangeListener(CarDrivingActivity.this);

                if (savedInstanceState == null) {
                    toggleGps(true);
                }
            }
        });
    }

    @UiThread
    public void toggleGps(boolean enableGps) {
        if (enableGps) {
            if ((ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) ||
                    (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED)) {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_COARSE_LOCATION, Manifest.permission.ACCESS_FINE_LOCATION}, PERMISSIONS_LOCATION);
            } else {
                enableLocation(true);
            }
        } else {
            enableLocation(false);
        }
    }

    private void enableLocation(boolean enabled) {
        if (enabled) {
            mapboxMap.setMyLocationEnabled(true);
            Location location = mapboxMap.getMyLocation();
            if (location != null) {
                setInitialPosition(new LatLng(location));
            }
        } else {
            mapboxMap.setMyLocationEnabled(false);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String permissions[], @NonNull int[] grantResults) {
        switch (requestCode) {
            case PERMISSIONS_LOCATION: {
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    enableLocation(true);
                }
            }
        }
    }

    private void setInitialPosition(LatLng latLng) {
        mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(
                new CameraPosition.Builder().target(latLng).zoom(15).tilt(20f).build()));
        mapboxMap.setMyLocationEnabled(true);

        TrackingSettings trackingSettings = mapboxMap.getTrackingSettings();
        trackingSettings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
        trackingSettings.setMyBearingTrackingMode(MyBearingTracking.GPS);
    }

    @Override
    public void onMyLocationChange(@Nullable Location location) {
        if (location != null) {
            if (this.location == null) {
                // initial location to reposition map
                setInitialPosition(new LatLng(location));
            }
            this.location = location;
        }
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
