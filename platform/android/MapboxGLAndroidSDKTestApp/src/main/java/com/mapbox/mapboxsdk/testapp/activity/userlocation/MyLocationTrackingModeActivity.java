package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.StringDef;
import android.support.design.widget.Snackbar;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.Toast;

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

public class MyLocationTrackingModeActivity extends AppCompatActivity implements MapboxMap.OnMyLocationChangeListener, AdapterView.OnItemSelectedListener {

    private MapView mMapView;
    private MapboxMap mMapboxMap;
    private Spinner mLocationSpinner, mBearingSpinner;
    private Location mLocation;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_location_tracking);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayShowTitleEnabled(false);
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                mMapboxMap = mapboxMap;

                // disable dismissal when a gesture occurs
                mMapboxMap.getTrackingSettings().setDismissTrackingOnGesture(false);
                mMapboxMap.getTrackingSettings().setDismissBearingTrackingOnGesture(false);

                mapboxMap.setOnMyLocationChangeListener(MyLocationTrackingModeActivity.this);

                ArrayAdapter<CharSequence> locationTrackingAdapter = ArrayAdapter.createFromResource(actionBar.getThemedContext(), R.array.user_tracking_mode, android.R.layout.simple_spinner_item);
                locationTrackingAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
                mLocationSpinner = (Spinner) findViewById(R.id.spinner_location);
                mLocationSpinner.setAdapter(locationTrackingAdapter);
                mLocationSpinner.setOnItemSelectedListener(MyLocationTrackingModeActivity.this);

                ArrayAdapter<CharSequence> bearingTrackingAdapter = ArrayAdapter.createFromResource(actionBar.getThemedContext(), R.array.user_bearing_mode, android.R.layout.simple_spinner_item);
                bearingTrackingAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
                mBearingSpinner = (Spinner) findViewById(R.id.spinner_bearing);
                mBearingSpinner.setAdapter(bearingTrackingAdapter);
                mBearingSpinner.setOnItemSelectedListener(MyLocationTrackingModeActivity.this);


                mapboxMap.setOnMyLocationTrackingModeChangeListener(new MapboxMap.OnMyLocationTrackingModeChangeListener() {
                    @Override
                    public void onMyLocationTrackingModeChange(@MyLocationTracking.Mode int myLocationTrackingMode) {
                        if (MyLocationTracking.TRACKING_NONE == myLocationTrackingMode) {
                            mLocationSpinner.setOnItemSelectedListener(null);
                            mLocationSpinner.setSelection(0);
                            mLocationSpinner.setOnItemSelectedListener(MyLocationTrackingModeActivity.this);
                        }
                    }
                });

                mapboxMap.setOnMyBearingTrackingModeChangeListener(new MapboxMap.OnMyBearingTrackingModeChangeListener() {
                    @Override
                    public void onMyBearingTrackingModeChange(@MyBearingTracking.Mode int myBearingTrackingMode) {
                        if (MyBearingTracking.NONE == myBearingTrackingMode) {
                            mBearingSpinner.setOnItemSelectedListener(null);
                            mBearingSpinner.setSelection(0);
                            mBearingSpinner.setOnItemSelectedListener(MyLocationTrackingModeActivity.this);
                        }
                    }
                });
            }
        });
    }

    @Override
    public void onMyLocationChange(@Nullable Location location) {
        if (location != null) {
            if (mLocation == null) {
                // initial location to reposition map
                mMapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(location.getLatitude(), location.getLongitude()), 14));
                mMapboxMap.setMyLocationEnabled(true);
                mLocationSpinner.setEnabled(true);
                mBearingSpinner.setEnabled(true);
            }
            mLocation = location;
            showSnackBar();
        }
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
        if(mLocation.hasAccuracy()){
            desc += String.format("Acc = %.0f m",mLocation.getAccuracy());
        }

        if (noInfo) {
            desc += "No extra info";
        }
        Snackbar.make(findViewById(android.R.id.content), desc, Snackbar.LENGTH_SHORT).show();
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) throws SecurityException {
        TrackingSettings trackingSettings = mMapboxMap.getTrackingSettings();
        if (parent.getId() == R.id.spinner_location) {
            switch (position) {
                case 0:
                    trackingSettings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_NONE);
                    break;

                case 1:
                    trackingSettings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
                    break;
            }
        } else if (parent.getId() == R.id.spinner_bearing) {
            switch (position) {
                case 0:
                    trackingSettings.setMyBearingTrackingMode(MyBearingTracking.NONE);
                    break;

                case 1:
                    trackingSettings.setMyBearingTrackingMode(MyBearingTracking.GPS);
                    break;

                case 2:
                    trackingSettings.setMyBearingTrackingMode(MyBearingTracking.COMPASS);
                    break;
            }
        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

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
        getMenuInflater().inflate(R.menu.menu_tracking, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        boolean state;
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            case R.id.action_toggle_dismissible_tracking:
                state = !item.isChecked();
                mMapboxMap.getTrackingSettings().setDismissLocationTrackingOnGesture(state);
                Toast.makeText(this, "Dismiss tracking mode on gesture = " + state, Toast.LENGTH_SHORT).show();
                item.setChecked(state);
                return true;
            case R.id.action_toggle_dismissible_bearing:
                state = !item.isChecked();
                mMapboxMap.getTrackingSettings().setDismissBearingTrackingOnGesture(state);
                Toast.makeText(this, "Dismiss bearing mode on gesture = " + state, Toast.LENGTH_SHORT).show();
                item.setChecked(state);
            default:
                return super.onOptionsItemSelected(item);
        }
    }

}
