package com.mapbox.mapboxsdk.testapp.activity;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.widget.Toast;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.ApiAccess;
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
                        .position(new LatLng(38.9002073, -77.03364419)));

                Marker marker = mapboxMap.addMarker(new MarkerOptions()
                        .title("White House")
                        .snippet("The official residence and principal workplace of the President of the United States, located at 1600 Pennsylvania Avenue NW in Washington, D.C. It has been the residence of every U.S. president since John Adams in 1800.")
                        .position(new LatLng(38.897705003219784, -77.03655168667463)));

                // open InfoWindow at startup!
                mapboxMap.selectMarker(marker);

                mapboxMap.addMarker(new MarkerOptions().title("Intersection")
                        .snippet("E St NW with 17th St NW")
                        .position(new LatLng(38.8954236, -77.0394623)));

                mapboxMap.setOnInfoWindowCloseListener(new MapboxMap.OnInfoWindowCloseListener() {
                    @Override
                    public void onInfoWindowClose(Marker marker) {
                        Toast.makeText(getApplicationContext(), "OnClose: " + marker.getTitle(), Toast.LENGTH_LONG).show();
                    }
                });

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


                mapboxMap.setOnInfoWindowClickListener(new MapboxMap.OnInfoWindowClickListener() {
                    @Override
                    public boolean onInfoWindowClick(@NonNull Marker marker) {
                        Toast.makeText(getApplicationContext(), "OnClick: " + marker.getTitle(), Toast.LENGTH_LONG).show();
                        // return false to close the info window
                        // return true to leave the info window open
                        return false;
                    }
                });

                mapboxMap.setOnInfoWindowLongClickListener(new MapboxMap.OnInfoWindowLongClickListener() {
                    @Override
                    public void onInfoWindowLongClick(Marker marker) {
                        Toast.makeText(getApplicationContext(), "OnLongClick: " + marker.getTitle(), Toast.LENGTH_LONG).show();
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
