package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.Manifest;
import android.content.pm.PackageManager;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.design.widget.Snackbar;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
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
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.TrackingSettings;
import com.mapbox.mapboxsdk.testapp.R;

public class MyLocationTrackingModeActivity extends AppCompatActivity
    implements MapboxMap.OnMyLocationChangeListener, AdapterView.OnItemSelectedListener {

    private MapView mapView;
    private MapboxMap mapboxMap;
    private Spinner locationSpinner;
    private Spinner bearingSpinner;
    private Location location;
    private static final int PERMISSIONS_LOCATION = 0;

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
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

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                MyLocationTrackingModeActivity.this.mapboxMap = mapboxMap;

                // disable dismissal when a gesture occurs
                TrackingSettings trackingSettings = mapboxMap.getTrackingSettings();
                trackingSettings.setDismissLocationTrackingOnGesture(false);
                trackingSettings.setDismissBearingTrackingOnGesture(false);

                mapboxMap.setOnMyLocationChangeListener(MyLocationTrackingModeActivity.this);

                ArrayAdapter<CharSequence> locationTrackingAdapter = ArrayAdapter.createFromResource(
                    actionBar.getThemedContext(), R.array.user_tracking_mode, android.R.layout.simple_spinner_item);
                locationTrackingAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
                locationSpinner = (Spinner) findViewById(R.id.spinner_location);
                locationSpinner.setAdapter(locationTrackingAdapter);
                locationSpinner.setOnItemSelectedListener(MyLocationTrackingModeActivity.this);

                ArrayAdapter<CharSequence> bearingTrackingAdapter = ArrayAdapter.createFromResource(
                    actionBar.getThemedContext(), R.array.user_bearing_mode, android.R.layout.simple_spinner_item);
                bearingTrackingAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
                bearingSpinner = (Spinner) findViewById(R.id.spinner_bearing);
                bearingSpinner.setAdapter(bearingTrackingAdapter);
                bearingSpinner.setOnItemSelectedListener(MyLocationTrackingModeActivity.this);

                mapboxMap.setOnMyLocationTrackingModeChangeListener(new MapboxMap.OnMyLocationTrackingModeChangeListener() {
                    @Override
                    public void onMyLocationTrackingModeChange(@MyLocationTracking.Mode int myLocationTrackingMode) {
                        if (MyLocationTracking.TRACKING_NONE == myLocationTrackingMode) {
                            locationSpinner.setOnItemSelectedListener(null);
                            locationSpinner.setSelection(0);
                            locationSpinner.setOnItemSelectedListener(MyLocationTrackingModeActivity.this);
                        }
                    }
                });

                mapboxMap.setOnMyBearingTrackingModeChangeListener(new MapboxMap.OnMyBearingTrackingModeChangeListener() {
                    @Override
                    public void onMyBearingTrackingModeChange(@MyBearingTracking.Mode int myBearingTrackingMode) {
                        if (MyBearingTracking.NONE == myBearingTrackingMode) {
                            bearingSpinner.setOnItemSelectedListener(null);
                            bearingSpinner.setSelection(0);
                            bearingSpinner.setOnItemSelectedListener(MyLocationTrackingModeActivity.this);
                        }
                    }
                });

                if (savedInstanceState == null) {
                    toggleGps(true);
                }
            }
        });
    }

    @UiThread
    public void toggleGps(boolean enableGps) {
        if (enableGps) {
            if ((ContextCompat.checkSelfPermission(this,
                Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED)
                || (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
                != PackageManager.PERMISSION_GRANTED)) {
                ActivityCompat.requestPermissions(this, new String[]{
                    Manifest.permission.ACCESS_COARSE_LOCATION,
                    Manifest.permission.ACCESS_FINE_LOCATION}, PERMISSIONS_LOCATION);
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
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == PERMISSIONS_LOCATION) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                enableLocation(true);
            }
        }
    }

    private void setInitialPosition(LatLng latLng) {
        mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(latLng, 14));
        mapboxMap.setMyLocationEnabled(true);
        locationSpinner.setEnabled(true);
        bearingSpinner.setEnabled(true);
    }

    @Override
    public void onMyLocationChange(@Nullable Location location) {
        if (location != null) {
            if (this.location == null) {
                // initial location to reposition map
                setInitialPosition(new LatLng(location));
            }
            this.location = location;
            showSnackBar();
        }
    }

    private void showSnackBar() {
        String desc = "Loc Chg: ";
        boolean noInfo = true;
        if (location.hasSpeed()) {
            desc += String.format(MapboxConstants.MAPBOX_LOCALE, "Spd = %.1f km/h ", location.getSpeed() * 3.6f);
            noInfo = false;
        }
        if (location.hasAltitude()) {
            desc += String.format(MapboxConstants.MAPBOX_LOCALE, "Alt = %.0f m ", location.getAltitude());
            noInfo = false;
        }
        if (location.hasAccuracy()) {
            desc += String.format(MapboxConstants.MAPBOX_LOCALE, "Acc = %.0f m", location.getAccuracy());
        }

        if (noInfo) {
            desc += "No extra info";
        }
        Snackbar.make(findViewById(android.R.id.content), desc, Snackbar.LENGTH_SHORT).show();
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) throws SecurityException {
        TrackingSettings trackingSettings = mapboxMap.getTrackingSettings();
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
        mapView.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        mapView.onPause();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mapView.onSaveInstanceState(outState);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mapView.onDestroy();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mapView.onLowMemory();
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
                mapboxMap.getTrackingSettings().setDismissLocationTrackingOnGesture(state);
                Toast.makeText(this, "Dismiss tracking mode on gesture = " + state, Toast.LENGTH_SHORT).show();
                item.setChecked(state);
                return true;
            case R.id.action_toggle_dismissible_bearing:
                state = !item.isChecked();
                mapboxMap.getTrackingSettings().setDismissBearingTrackingOnGesture(state);
                Toast.makeText(this, "Dismiss bearing mode on gesture = " + state, Toast.LENGTH_SHORT).show();
                item.setChecked(state);
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

}
