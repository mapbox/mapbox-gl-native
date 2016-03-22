package com.mapbox.mapboxsdk.testapp.activity;

import android.graphics.PointF;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.ApiAccess;
import com.mapbox.mapboxsdk.maps.MapView;

import java.text.DecimalFormat;
import java.util.ArrayList;

public class PressForMarkerActivity extends AppCompatActivity implements MapboxMap.OnMapLongClickListener {

    private static final DecimalFormat LAT_LON_FORMATTER = new DecimalFormat("#.#####");
    private static final String STATE_MARKER_LIST = "markerList";

    private MapView mMapView;
    private MapboxMap mMapboxMap;
    private ArrayList<MarkerOptions> mMarkerList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_press_for_marker);

        Toolbar toolbar = (Toolbar) findViewById(R.id.secondToolBar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        // Adding MapView programmatically
        mMapView = new MapView(this);
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.setStyle(Style.EMERALD);
        mMapView.onCreate(savedInstanceState);
        ((ViewGroup) findViewById(R.id.activity_container)).addView(mMapView);

        if (savedInstanceState != null) {
            mMarkerList = savedInstanceState.getParcelableArrayList(STATE_MARKER_LIST);
        }else{
            mMarkerList = new ArrayList<>();
        }

        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                mMapboxMap = mapboxMap;

                // set camera
                mapboxMap.setCameraPosition(new CameraPosition.Builder()
                        .target(new LatLng(45.1855569, 5.7215506))
                        .zoom(11)
                        .build());

                // add markers
                mapboxMap.addMarkers(mMarkerList);

                // set map long click listeners
                mapboxMap.setOnMapLongClickListener(PressForMarkerActivity.this);
            }
        });
    }

    @Override
    public void onMapLongClick(@NonNull LatLng point) {
        PointF pixel = mMapboxMap.getProjection().toScreenLocation(point);
        String title = LAT_LON_FORMATTER.format(point.getLatitude()) + ", " + LAT_LON_FORMATTER.format(point.getLongitude());
        String snippet = "X = " + (int) pixel.x + ", Y = " + (int) pixel.y;

        MarkerOptions marker = new MarkerOptions()
                .position(point)
                .title(title)
                .snippet(snippet);

        mMarkerList.add(marker);
        mMapboxMap.addMarker(marker);
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
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mMapView.onSaveInstanceState(outState);
        outState.putParcelableArrayList(STATE_MARKER_LIST, mMarkerList);
    }

    @Override
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
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
