package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.location.Location;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

public class MyLocationDotColor extends AppCompatActivity implements LocationListener {

    private MapView mapView;
    private MapboxMap map;
    private Location location;
    private boolean firstRun;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_location_dot_color);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        location = LocationServices.getLocationServices(this).getLastLocation();

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.setAccessToken(getString(R.string.mapbox_access_token));
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(MapboxMap mapboxMap) {

                map = mapboxMap;

                mapboxMap.setMyLocationEnabled(true);
                mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom((location != null ? new LatLng(location) : new LatLng(0, 0)), 15));

            }
        });

        // handle default button clicks
        findViewById(R.id.defaultUserDotColoringButton).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                map.getMyLocationViewSettings().setAccuracyTintColor(ContextCompat.getColor(MyLocationDotColor.this, R.color.my_location_ring));
                map.getMyLocationViewSettings().setForegroundTintColor(ContextCompat.getColor(MyLocationDotColor.this, R.color.primaryDark));

            }
        });

        // handle tint user dot button clicks
        findViewById(R.id.tintUserDotButton).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                map.getMyLocationViewSettings().setAccuracyTintColor(ContextCompat.getColor(MyLocationDotColor.this, R.color.mapbox_green));
                map.getMyLocationViewSettings().setForegroundTintColor(ContextCompat.getColor(MyLocationDotColor.this, R.color.mapbox_green));

            }
        });

        // handle tint accuracy ring button clicks
        findViewById(R.id.UserAccuracyRingTintButton).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                map.getMyLocationViewSettings().setAccuracyTintColor(ContextCompat.getColor(MyLocationDotColor.this, R.color.accent));
                map.getMyLocationViewSettings().setForegroundTintColor(ContextCompat.getColor(MyLocationDotColor.this, R.color.mapbox_blue));

            }
        });

        LocationServices.getLocationServices(this).addLocationListener(this);

    }

    @Override
    public void onLocationChanged(Location location) {
        if (map != null && firstRun) {
            map.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(location), 15));
            firstRun = false;
        }
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
    public void onLowMemory() {
        super.onLowMemory();
        mapView.onLowMemory();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mapView.onDestroy();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mapView.onSaveInstanceState(outState);
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
