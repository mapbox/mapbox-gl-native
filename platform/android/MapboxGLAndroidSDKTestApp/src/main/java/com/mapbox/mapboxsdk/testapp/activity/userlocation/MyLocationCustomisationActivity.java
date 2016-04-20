package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.graphics.Color;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.design.widget.Snackbar;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationViewSettings;
import com.mapbox.mapboxsdk.testapp.R;

public class MyLocationCustomisationActivity extends AppCompatActivity implements LocationListener, OnMapReadyCallback {

    private MapView mMapView;
    private MapboxMap mMapboxMap;
    private Location mLocation;

    private Bundle savedState;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_location_customisation);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        savedState = savedInstanceState;

        LocationServices locationServices = LocationServices.getLocationServices(this);
        locationServices.addLocationListener(this);
        locationServices.toggleGPS(true);
    }

    @Override
    public void onLocationChanged(Location location) {
        if (location != null) {
            if (mMapView == null) {
                findViewById(R.id.progress).setVisibility(View.GONE);

                MapboxMapOptions mapboxMapOptions = new MapboxMapOptions();
                mapboxMapOptions.accessToken(getString(R.string.mapbox_access_token));
                mapboxMapOptions.styleUrl(Style.MAPBOX_STREETS);
                mapboxMapOptions.locationEnabled(true);
                mapboxMapOptions.camera(new CameraPosition.Builder()
                        .target(new LatLng(location))
                        .zoom(11)
                        .tilt(25)
                        .build());

                mMapView = new MapView(this, mapboxMapOptions);
                ((ViewGroup) findViewById(R.id.container)).addView(mMapView);

                mMapView.onCreate(savedState);
                mMapView.getMapAsync(this);
                mMapView.onResume();
                mLocation = location;
            }
        }
    }

    @Override
    public void onMapReady(MapboxMap mapboxMap) {
        MyLocationViewSettings locationViewSettings = mapboxMap.getMyLocationViewSettings();

        locationViewSettings.setBackgroundTintColor(Color.YELLOW);
        locationViewSettings.setBackgroundOffset(32, 32, 0, 0);

        locationViewSettings.setForegroundTintColor(Color.GREEN);

        locationViewSettings.setAccuracyTintColor(Color.RED);
        locationViewSettings.setAccuracyAlpha(100);
    }

    private void showSnackBar() {
        String desc = "Loc Chg: ";
        boolean noInfo = true;
        if (mLocation.hasSpeed()) {
            desc += String.format("Spd = %.1f km/h ", mLocation.getSpeed() * 3.6f);
            noInfo = false;
        }
        if (mLocation.hasAltitude()) {
            desc += String.format("Alt = %.0f m ", mLocation.getAltitude());
            noInfo = false;
        }
        if (noInfo) {
            desc += "No extra info";
        }
        Snackbar.make(findViewById(android.R.id.content), desc, Snackbar.LENGTH_SHORT).show();
    }

    @Override
    public void onResume() {
        super.onResume();
        if (mMapView != null) {
            mMapView.onResume();
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mMapView != null) {
            mMapView.onPause();
        }
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        if (mMapView != null) {
            mMapView.onSaveInstanceState(outState);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mMapView != null) {
            mMapView.onDestroy();
        }
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        if (mMapView != null) {
            mMapView.onLowMemory();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_tracking, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            case R.id.action_toggle_dismissible_tracking:
                boolean state = !item.isChecked();
                mMapboxMap.getTrackingSettings().setDismissTrackingOnGesture(state);
                Toast.makeText(this, "Dismiss tracking mode on gesture = " + state, Toast.LENGTH_SHORT).show();
                item.setChecked(state);
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

}