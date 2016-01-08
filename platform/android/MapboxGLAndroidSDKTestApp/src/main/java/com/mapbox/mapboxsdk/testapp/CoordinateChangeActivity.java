package com.mapbox.mapboxsdk.testapp;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;

import com.mapbox.mapboxsdk.MapFragment;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngZoom;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;
import com.mapbox.mapboxsdk.views.MapboxMap;
import com.mapbox.mapboxsdk.views.OnMapReadyCallback;

public class CoordinateChangeActivity extends AppCompatActivity implements OnMapReadyCallback {

    private boolean mFirstLatLng = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_animate_coordinate);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        MapFragment mapFragment = (MapFragment) getSupportFragmentManager().findFragmentById(R.id.mapFragment);
        mapFragment.getMapAsync(this);
    }

    @Override
    public void onMapReady(@NonNull final MapboxMap mapboxMap) {
        mapboxMap.setStyle(Style.MAPBOX_STREETS);
        mapboxMap.setLatLng(new LatLngZoom(38.87031, -77.00897, 16));
        mapboxMap.setCompassEnabled(false);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setColorFilter(ContextCompat.getColor(this, R.color.primary));
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                 mapboxMap.setLatLng(getLatLng(), true);
            }
        });
    }

    private LatLng getLatLng() {
        mFirstLatLng = !mFirstLatLng;
        return !mFirstLatLng ? new LatLng(38.87000, -77.00800) : new LatLng(38.87031, -77.00897);
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