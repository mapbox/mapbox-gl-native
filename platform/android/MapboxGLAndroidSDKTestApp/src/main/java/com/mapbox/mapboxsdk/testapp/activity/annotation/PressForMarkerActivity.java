package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.graphics.PointF;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import java.text.DecimalFormat;
import java.util.ArrayList;

public class PressForMarkerActivity extends AppCompatActivity {

    private MapView mapView;
    private MapboxMap mapboxMap;
    private ArrayList<MarkerOptions> mMarkerList = new ArrayList<>();

    private static final DecimalFormat LAT_LON_FORMATTER = new DecimalFormat("#.#####");

    private static String STATE_MARKER_LIST = "markerList";

    @Override
    protected void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_press_for_marker);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(final MapboxMap map) {
                mapboxMap = map;
                resetMap();

                mapboxMap.setOnMapLongClickListener(new MapboxMap.OnMapLongClickListener() {
                    @Override
                    public void onMapLongClick(@NonNull LatLng point) {
                        final PointF pixel = mapboxMap.getProjection().toScreenLocation(point);

                        String title = LAT_LON_FORMATTER.format(point.getLatitude()) + ", " + LAT_LON_FORMATTER.format(point.getLongitude());
                        String snippet = "X = " + (int) pixel.x + ", Y = " + (int) pixel.y;

                        MarkerOptions marker = new MarkerOptions()
                                .position(point)
                                .title(title)
                                .snippet(snippet);

                        mMarkerList.add(marker);
                        mapboxMap.addMarker(marker);
                    }
                });

                if (savedInstanceState != null) {
                    mMarkerList = savedInstanceState.getParcelableArrayList(STATE_MARKER_LIST);
                    mapboxMap.addMarkers(mMarkerList);
                }
            }
        });
    }

    private void resetMap() {
        if (mapboxMap == null) {
            return;
        }
        mapboxMap.removeAnnotations();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_press_for_marker, menu);
        return true;
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);

        mapView.onSaveInstanceState(outState);
        outState.putParcelableArrayList(STATE_MARKER_LIST, mMarkerList);
    }

    @Override
    public void onResume() {
        super.onResume();
        mapView.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        mapView.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mapView.onDestroy();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mapView.onLowMemory();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            case R.id.menuItemReset:
                resetMap();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }
}
