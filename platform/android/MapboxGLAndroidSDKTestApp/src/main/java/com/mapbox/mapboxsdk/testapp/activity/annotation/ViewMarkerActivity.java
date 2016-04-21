package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.os.Bundle;
import android.support.annotation.DrawableRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.view.CustomMarkerView;

public class ViewMarkerActivity extends AppCompatActivity implements OnMapReadyCallback {

    private MapView mMapView;
    private Marker brazil, germany, china, us;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_marker_view);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(this);
    }

    @Override
    public void onMapReady(MapboxMap mapboxMap) {

        // adapt GL-marker to View-marker
        mapboxMap.setMarkerViewAdapter(new MapboxMap.MarkerViewAdapter() {
            @Nullable
            @Override
            public MarkerView getView(@NonNull Marker marker) {
                if (marker.equals(brazil)) {
                    return createMarkerView("br", R.drawable.ic_brazil);
                } else if (marker.equals(germany)) {
                    return createMarkerView("de", R.drawable.ic_germany);
                } else if (marker.equals(china)) {
                    return createMarkerView("ch", R.drawable.ic_china);
                } else if (marker.equals(us)) {
                    return createMarkerView("us", R.drawable.ic_us);
                }
                return null;
            }
        });

        // add markers
        china = mapboxMap.addMarker(new MarkerOptions().position(new LatLng(31.230416, 121.473701)));
        brazil = mapboxMap.addMarker(new MarkerOptions().position(new LatLng(-15.798200, -47.922363)));
        us = mapboxMap.addMarker(new MarkerOptions().position(new LatLng(38.907192, -77.036871)));
        germany = mapboxMap.addMarker(new MarkerOptions().position(new LatLng(52.520007, 13.404954)));
    }

    private MarkerView createMarkerView(String countryAbbrev, @DrawableRes int countryIconRes) {
        CustomMarkerView customMarkerView = new CustomMarkerView(this);
        customMarkerView.setText(countryAbbrev);
        customMarkerView.setImage(countryIconRes);
        return customMarkerView;
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

}