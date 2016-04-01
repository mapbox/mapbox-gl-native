package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.maps.MapView;

public class LatLngBoundsActivity extends AppCompatActivity {

    private static final LatLng LOS_ANGELES = new LatLng(34.053940, -118.242622);
    private static final LatLng NEW_YORK = new LatLng(40.712730, -74.005953);

    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_visible_bounds);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.setAccessToken(getString(R.string.mapbox_access_token));
        mMapView.setStyle(Style.DARK);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull final MapboxMap mapboxMap) {

                UiSettings uiSettings = mapboxMap.getUiSettings();
                uiSettings.setAllGesturesEnabled(false);

                mapboxMap.addMarker(new MarkerOptions()
                        .title("Los Angeles")
                        .snippet("City Hall")
                        .position(LOS_ANGELES));

                mapboxMap.addMarker(new MarkerOptions()
                        .title("New York")
                        .snippet("City Hall")
                        .position(NEW_YORK));

                // Create Bounds
                final LatLngBounds bounds = new LatLngBounds.Builder()
                        .include(NEW_YORK)
                        .include(LOS_ANGELES)
                        .build();

                // And padding
                int padding = (int) getResources().getDimension(R.dimen.coordinatebounds_margin);

                // Move camera to the bounds with padding
                mapboxMap.animateCamera(CameraUpdateFactory.newLatLngBounds(bounds, padding));

                // Call mapboxMap.getProjection().getVisibleRegion().latLngBounds to retrieve the bounds
                Log.v(MapboxConstants.TAG, mapboxMap.getProjection().getVisibleRegion().latLngBounds.toString());
            }
        });


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
