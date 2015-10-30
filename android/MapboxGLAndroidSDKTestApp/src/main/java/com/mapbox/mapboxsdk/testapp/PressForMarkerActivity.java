package com.mapbox.mapboxsdk.testapp;

import android.graphics.PointF;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.AppCompatMapActivity;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.views.MapView;
import java.text.DecimalFormat;

public class PressForMarkerActivity extends AppCompatMapActivity implements MapView.OnMapLongClickListener {

    private static final DecimalFormat latLonFormatter = new DecimalFormat("#.#####");

    public PressForMarkerActivity() {
        super(null, Style.EMERALD);
    }

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

        MapView mapView = getMap();
        mapView.setCenterCoordinate(new LatLng(45.1855569, 5.7215506));
        mapView.setZoomLevel(11);
        mapView.setOnMapLongClickListener(this);
        ((ViewGroup) findViewById(R.id.activity_container)).addView(mapView);
    }

    @Override
    public void onMapLongClick(@NonNull LatLng point) {
        MapView mapView = getMap();
        final PointF pixel = mapView.toScreenLocation(point);

        String title = latLonFormatter.format(point.getLatitude()) + ", " + latLonFormatter.format(point.getLongitude());
        String snippet = "X = " + (int) pixel.x + ", Y = " + (int) pixel.y;

        mapView.addMarker(new MarkerOptions()
                .position(point)
                .title(title)
                .snippet(snippet));
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
