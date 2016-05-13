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

    private int cycleStep;
    private int cycle;
    private Handler cycleHandler;
    private Runnable cycleRunner = new Runnable() {
        @Override
        public void run() {
            startAddRemoveCycle();
        }
    };

    private final static LatLng[] LAT_LNGS = new LatLng[]{
            new LatLng(38.907327, -77.041293),
            new LatLng(38.909698, -77.029642),
            new LatLng(38.907227, -77.036530),
            new LatLng(38.905607, -77.031916),
            new LatLng(38.897424, -77.036508),
            new LatLng(38.897642, -77.041980),
            new LatLng(38.889876, -77.008849),
            new LatLng(38.889441, -77.050134),
            new LatLng(38.902580, -77.050102)
    };

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
                cycleHandler = new Handler();
                startAddRemoveCycle();
            }
        });
    }

    public void startAddRemoveCycle() {
        switch (cycleStep) {
            case 0:
                // removing old annotations
                mMapboxMap.removeAnnotations();
                break;

            case 1:
                // addMarker
                for (LatLng latLng : LAT_LNGS) {
                    Marker marker = mMapboxMap.addMarker(new MarkerOptions().position(latLng));
                    Log.v(MapboxConstants.TAG, "Marker added via addMarker " + marker.getId());
                }
                break;

            case 2:
                // removing old annotations
                mMapboxMap.removeAnnotations(mMapboxMap.getAnnotations());
                break;

            case 3:
                // addMarkers
                List<MarkerOptions> markerOptions = new ArrayList<>();
                for (LatLng latLng : LAT_LNGS) {
                    markerOptions.add(new MarkerOptions().position(latLng));
                }
                List<Marker> markers = mMapboxMap.addMarkers(markerOptions);
                for (Marker m : markers) {
                    Log.v(MapboxConstants.TAG, "Marker added via addMarkers " + m.getId());
                }
                break;

            case 4:
                // removing old annotations
                List<Annotation> annotationList = mMapboxMap.getAnnotations();
                for (Annotation annotation : annotationList) {
                    mMapboxMap.removeAnnotation(annotation.getId());
                }
                break;

            case 5:
                // addMarker
                for (LatLng latLng : LAT_LNGS) {
                    Marker marker = mMapboxMap.addMarker(new MarkerOptions().position(latLng));
                    Log.v(MapboxConstants.TAG, "Marker added via addMarker " + marker.getId());
                }
                break;
        }

        // update cycle
        if (cycleStep == 5) {
            cycle++;
            cycleStep = 0;
        } else {
            cycleStep++;
        }

        // show data
        Toast.makeText(this, "Cycle " + cycle + ", step " + cycleStep, Toast.LENGTH_SHORT).show();

        // schedule next cycle
        cycleHandler.postDelayed(cycleRunner, 1500);
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
        cycleHandler.removeCallbacks(cycleRunner);
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
