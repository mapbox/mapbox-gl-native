package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.graphics.Color;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

public class MyLocationCustomizationActivity extends AppCompatActivity implements LocationListener {

    private MapView mapView;
    private MapboxMap mapboxMap;
    private Location location;
    private boolean firstRun;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_location_customization);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        findViewById(R.id.progress).setVisibility(View.GONE);
        location = LocationServices.getLocationServices(this).getLastLocation();

        MapboxMapOptions mapboxMapOptions = new MapboxMapOptions();
        mapboxMapOptions.accessToken(getString(R.string.mapbox_access_token));
        mapboxMapOptions.styleUrl(Style.MAPBOX_STREETS);
        mapboxMapOptions.locationEnabled(true);
        mapboxMapOptions.camera(new CameraPosition.Builder()
                .target(location != null ? new LatLng(location) : new LatLng(0, 0))
                .zoom(11)
                .tilt(25)
                .build());

        mapboxMapOptions.myLocationForegroundDrawables(ContextCompat.getDrawable(this, R.drawable.ic_chelsea),
                ContextCompat.getDrawable(this, R.drawable.ic_chelsea));
        mapboxMapOptions.myLocationBackgroundDrawable(ContextCompat.getDrawable(this, R.drawable.ic_arsenal));
        mapboxMapOptions.myLocationForegroundTintColor(Color.GREEN);
        mapboxMapOptions.myLocationBackgroundTintColor(Color.YELLOW);
        mapboxMapOptions.myLocationBackgroundPadding(new int[]{0, 0,
                (int) getResources().getDimension(R.dimen.locationview_background_drawable_padding),
                (int) getResources().getDimension(R.dimen.locationview_background_drawable_padding)});

        mapboxMapOptions.myLocationAccuracyTint(Color.RED);
        mapboxMapOptions.myLocationAccuracyAlpha(155);

        mapView = new MapView(this, mapboxMapOptions);
        ViewGroup parent = (ViewGroup) findViewById(R.id.container);
        parent.addView(mapView);

        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(MapboxMap map) {
                mapboxMap = map;
            }
        });

        LocationServices.getLocationServices(this).addLocationListener(this);
    }

    @Override
    public void onLocationChanged(Location location) {
        if (mapboxMap != null && firstRun) {
            mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(location), 10));
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
