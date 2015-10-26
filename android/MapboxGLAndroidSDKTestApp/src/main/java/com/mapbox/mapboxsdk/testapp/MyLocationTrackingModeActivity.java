package com.mapbox.mapboxsdk.testapp;

import android.location.Location;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.design.widget.Snackbar;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;

public class MyLocationTrackingModeActivity extends AppCompatActivity implements MapView.OnMyLocationChangeListener, AdapterView.OnItemSelectedListener {

    private MapView mMapView;
    private Spinner mSpinner;
    private Location mLocation;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_location_tracking);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayShowTitleEnabled(false);
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        ArrayAdapter<CharSequence> spinnerAdapter = ArrayAdapter.createFromResource(actionBar.getThemedContext(), R.array.user_tracking_mode, android.R.layout.simple_spinner_item);
        spinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mSpinner = (Spinner) findViewById(R.id.spinner);
        mSpinner.setAdapter(spinnerAdapter);
        mSpinner.setOnItemSelectedListener(this);
        mSpinner.setEnabled(false);

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.onCreate(savedInstanceState);
        mMapView.setOnMyLocationChangeListener(this);
        mMapView.setMyLocationEnabled(true);
    }

    @Override
    public void onMyLocationChange(@Nullable Location location) {
        if (location != null) {
            if (mLocation == null) {
                // initial location to reposition map
                mMapView.setCenterCoordinate(new LatLng(location.getLatitude(), location.getLongitude()));
                mSpinner.setEnabled(true);
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
        if (noInfo) {
            desc += "No extra info";
        }
        Snackbar.make(findViewById(android.R.id.content), desc, Snackbar.LENGTH_SHORT).show();
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        switch (position) {
            case 0:
                mMapView.setMyLocationTrackingMode(MyLocationTracking.TRACKING_NONE);
                break;

            case 1:
                mMapView.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
                break;

//            case 2:
//                mMapView.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW_BEARING_GPS);
//                break;
//
//            case 3:
//                mMapView.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW_BEARING_COMPASS);
//                break;
//
//            case 4:
//                mMapView.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW_BEARING);
//                break;
        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }

    @Override
    protected void onStart() {
        super.onStart();
        mMapView.onStart();
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
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
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