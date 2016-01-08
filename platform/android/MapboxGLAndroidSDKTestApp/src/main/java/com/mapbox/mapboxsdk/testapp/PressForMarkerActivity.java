package com.mapbox.mapboxsdk.testapp;

import android.graphics.PointF;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.MapFragment;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;
import com.mapbox.mapboxsdk.views.MapboxMap;
import com.mapbox.mapboxsdk.views.OnMapReadyCallback;

import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

public class PressForMarkerActivity extends AppCompatActivity implements OnMapReadyCallback, MapboxMap.OnMapLongClickListener {

    private MapboxMap mMapboxMap;
    private ArrayList<MarkerOptions> mMarkerList;
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

        if (savedInstanceState != null) {
            mMarkerList = savedInstanceState.getParcelableArrayList(STATE_MARKER_LIST);
        }else{
            mMarkerList = new ArrayList<>();
        }

        MapFragment mapFragment = (MapFragment) getSupportFragmentManager().findFragmentById(R.id.mapFragment);
        mapFragment.getMapAsync(this);
    }

    @Override
    public void onMapReady(@NonNull MapboxMap mapboxMap) {
        mMapboxMap = mapboxMap;
        mMapboxMap.setStyleUrl(Style.EMERALD);
        mMapboxMap.setLatLng(new LatLng(45.1855569, 5.7215506));
        mMapboxMap.setZoom(11);
        mMapboxMap.setOnMapLongClickListener(this);
        mapboxMap.addMarkers(mMarkerList);
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putParcelableArrayList(STATE_MARKER_LIST, mMarkerList);
    }

    @Override
    public void onMapLongClick(@NonNull LatLng point) {
        final PointF pixel = mMapboxMap.toScreenLocation(point);

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
