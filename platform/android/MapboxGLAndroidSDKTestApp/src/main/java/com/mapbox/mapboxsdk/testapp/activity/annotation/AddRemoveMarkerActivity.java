package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.widget.Toast;

import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.List;

public class AddRemoveMarkerActivity extends AppCompatActivity {

    private MapboxMap mMapboxMap;
    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_marker_add_remove);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.setAccessToken(getString(R.string.mapbox_access_token));
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                mMapboxMap = mapboxMap;
                reloadMarkers();
            }
        });
    }

    private void reloadMarkers() {
        mMapboxMap.removeAnnotations();

        double lat = -80; // to 80
        double lon = -179; // to 179

        LatLng latLng = new LatLng();

        int verticalAmount = 12;
        int horizontalAmount = 50;
        double latStep = 2 * (Math.abs(lat) / verticalAmount);
        double lonStep = 2 * (Math.abs(lon) / horizontalAmount);
        Log.v(MapboxConstants.TAG, "LatStep " + latStep + " LonStep " + lonStep);

        for (int j = 0; j < verticalAmount; j++) {

            for (int i = 0; i < horizontalAmount; i++) {
                lon += lonStep;

                Log.v(MapboxConstants.TAG, "Lat " + lat + " Lon " + lon);

                latLng.setLatitude(lat);
                latLng.setLongitude(lon);

                MarkerOptions options = new MarkerOptions()
                        .position(latLng)
                        .icon(IconFactory.getInstance(AddRemoveMarkerActivity.this).fromResource(R.drawable.ic_chelsea));
                Marker m = mMapboxMap.addMarker(options);
                Log.v(MapboxConstants.TAG,"Marker aded with id "+m.getId());
            }

            lat += latStep;
            lon = -179;
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

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                reloadMarkers();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }
}
