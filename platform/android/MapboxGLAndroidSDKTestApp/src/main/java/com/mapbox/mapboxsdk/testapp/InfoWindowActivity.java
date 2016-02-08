package com.mapbox.mapboxsdk.testapp;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.maps.MapView;

import java.text.DecimalFormat;

public class InfoWindowActivity extends AppCompatActivity {

    private MapView mMapView;
    private Marker mCustomMarker;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_infowindow);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull final MapboxMap mapboxMap) {
                mapboxMap.addMarker(new MarkerOptions()
                        .title("Intersection")
                        .snippet("H St NW with 15th St NW")
                        .icon(IconFactory.getInstance(InfoWindowActivity.this)
                                .fromAsset("london-underground-24.png"))
                        .position(new LatLng(38.9002073, -77.03364419)));

                mapboxMap.addMarker(new MarkerOptions()
                        .title("White House")
                        .snippet("The official residence and principal workplace of the President of the United States, located at 1600 Pennsylvania Avenue NW in Washington, D.C. It has been the residence of every U.S. president since John Adams in 1800.")
                        .icon(IconFactory.getInstance(InfoWindowActivity.this).fromAsset("town-hall-24.png"))
                        .position(new LatLng(38.897705003219784, -77.03655168667463)));

                mapboxMap.addMarker(new MarkerOptions().title("Intersection")
                        .snippet("E St NW with 17th St NW")
                        .icon(IconFactory.getInstance(InfoWindowActivity.this).fromAsset("commercial-24.png"))
                        .position(new LatLng(38.8954236, -77.0394623)));

                final DecimalFormat formatter = new DecimalFormat("#.#####");
                mapboxMap.setOnMapLongClickListener(new MapboxMap.OnMapLongClickListener() {
                    @Override
                    public void onMapLongClick(@NonNull LatLng point) {
                        // Remove previous added marker
                        if (mCustomMarker != null) {
                            mapboxMap.removeAnnotation(mCustomMarker);
                            mCustomMarker = null;
                        }

                        // Add marker on long click location with default marker image
                        mCustomMarker = mapboxMap.addMarker(new MarkerOptions()
                                .title("Custom Marker")
                                .snippet(formatter.format(point.getLatitude()) + ", " + formatter.format(point.getLongitude()))
                                .position(point));
                    }
                });
            }
        });
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
