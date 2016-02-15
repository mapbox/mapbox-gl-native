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

import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.CameraUpdateFactory;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.utils.ApiAccess;

public class DynamicMarkerChangeActivity extends AppCompatActivity {

    private static final LatLng LAT_LNG_CHELSEA = new LatLng(51.481670, -0.190849);
    private static final LatLng LAT_LNG_ARSENAL = new LatLng(51.555062, -0.108417);

    private MapView mMapView;
    private MapboxMap mMapboxMap;
    private IconFactory mIconFactory;
    private Marker mMarker;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dynamic_marker);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mIconFactory = IconFactory.getInstance(this);

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.setTag(true);
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                mMapboxMap = mapboxMap;
                mapboxMap.setStyle(Style.MAPBOX_STREETS);
                mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(51.506675, -0.128699), 10));

                // Create marker
                MarkerOptions markerOptions = new MarkerOptions()
                        .position(LAT_LNG_CHELSEA)
                        .icon(mIconFactory.fromResource(R.drawable.ic_chelsea));
                mMarker = mapboxMap.addMarker(markerOptions);
            }
        });


        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setColorFilter(ContextCompat.getColor(this, R.color.primary));
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mMapboxMap != null) {
                    updateMarker();
                }
            }
        });
    }

    private void updateMarker() {
        // update model
        boolean first = (boolean) mMapView.getTag();
        mMapView.setTag(!first);

        // update marker
        mMarker.setPosition(first ? LAT_LNG_CHELSEA : LAT_LNG_ARSENAL);
        mMarker.setIcon(mIconFactory.fromResource(first ? R.drawable.ic_chelsea : R.drawable.ic_arsenal));
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
