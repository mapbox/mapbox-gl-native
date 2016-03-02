package com.mapbox.mapboxsdk.testapp;

import android.graphics.Point;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.utils.ApiAccess;

public class MaxMinZoomActivity extends AppCompatActivity {

    private MapboxMap mMapboxMap;
    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maxmin_zoom);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.manualZoomMapView);
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.setStyle(Style.SATELLITE_STREETS);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull final MapboxMap mapboxMap) {
                mMapboxMap = mapboxMap;

                UiSettings uiSettings = mapboxMap.getUiSettings();
                uiSettings.setMinZoom(3);
                uiSettings.setMaxZoom(5);

                mMapboxMap.moveCamera(CameraUpdateFactory.newLatLng(new LatLng(-1.063510, 32.895425)));
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_zoom, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {

            case android.R.id.home:
                onBackPressed();
                return true;

            case R.id.action_zoom_in:
                mMapboxMap.animateCamera(CameraUpdateFactory.zoomIn());
                return true;

            case R.id.action_zoom_out:
                mMapboxMap.animateCamera(CameraUpdateFactory.zoomOut());
                return true;

            case R.id.action_zoom_by:
                mMapboxMap.animateCamera(CameraUpdateFactory.zoomBy(2));
                return true;
            case R.id.action_zoom_to:
                mMapboxMap.animateCamera(CameraUpdateFactory.zoomTo(2));
                return true;

            case R.id.action_zoom_to_point:
                mMapboxMap.animateCamera(CameraUpdateFactory.zoomBy(12, new Point(100, 100)));
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
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
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
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
