package com.mapbox.mapboxsdk.testapp;

import android.graphics.PointF;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;

import java.text.DecimalFormat;
import java.util.ArrayList;

public class PressForMarkerActivity extends AppCompatActivity implements MapView.OnMapLongClickListener {

    private MapView mMapView;
    private ArrayList<MarkerOptions> mMarkerList = new ArrayList<>();

    private static final DecimalFormat LAT_LON_FORMATTER = new DecimalFormat("#.#####");

    private static String STATE_MARKER_LIST = "markerList";

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

        mMapView = new MapView(this, ApiAccess.getToken(this));
        mMapView.setStyleUrl(Style.EMERALD);
        mMapView.onCreate(savedInstanceState);
        mMapView.setCenterCoordinate(new LatLng(45.1855569, 5.7215506));
        mMapView.setZoomLevel(11);
        mMapView.setOnMapLongClickListener(this);
        ((ViewGroup) findViewById(R.id.activity_container)).addView(mMapView);

        if (savedInstanceState != null) {
            mMarkerList = savedInstanceState.getParcelableArrayList(STATE_MARKER_LIST);
            mMapView.addMarkers(mMarkerList);
        }
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);

        mMapView.onSaveInstanceState(outState);
        outState.putParcelableArrayList(STATE_MARKER_LIST, mMarkerList);
    }

    @Override
    public void onMapLongClick(@NonNull LatLng point) {
        final PointF pixel = mMapView.toScreenLocation(point);

        String title = LAT_LON_FORMATTER.format(point.getLatitude()) + ", " + LAT_LON_FORMATTER.format(point.getLongitude());
        String snippet = "X = " + (int) pixel.x + ", Y = " + (int) pixel.y;

        MarkerOptions marker = new MarkerOptions()
                .position(point)
                .title(title)
                .snippet(snippet);

        mMarkerList.add(marker);
        mMapView.addMarker(marker);
    }

    /**
     * Dispatch onStart() to all fragments.  Ensure any created loaders are
     * now started.
     */
    @Override
    protected void onStart() {
        super.onStart();
        mMapView.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
    }

    // Called when our app goes into the background
    @Override
    public void onPause() {
        super.onPause();

        mMapView.onPause();
    }

    // Called when our app comes into the foreground
    @Override
    public void onResume() {
        super.onResume();

        mMapView.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }

    // Called when the system is running low on memory
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
