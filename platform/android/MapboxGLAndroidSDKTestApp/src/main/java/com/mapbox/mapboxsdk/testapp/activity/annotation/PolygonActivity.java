package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.List;

public class PolygonActivity extends AppCompatActivity implements OnMapReadyCallback {

    private static final int BLUE_COLOR = Color.parseColor("#3bb2d0");
    private static final int RED_COLOR = Color.parseColor("#AF0000");

    private static final float FULL_ALPHA = 1.0f;
    private static final float PARTIAL_ALPHA = 0.5f;
    private static final float NO_ALPHA = 0.0f;

    private static final List<LatLng> POINTS = new ArrayList<LatLng>() {{
        add(new LatLng(45.522585, -122.685699));
        add(new LatLng(45.534611, -122.708873));
        add(new LatLng(45.530883, -122.678833));
        add(new LatLng(45.547115, -122.667503));
        add(new LatLng(45.530643, -122.660121));
        add(new LatLng(45.533529, -122.636260));
        add(new LatLng(45.521743, -122.659091));
        add(new LatLng(45.510677, -122.648792));
        add(new LatLng(45.515008, -122.664070));
        add(new LatLng(45.502496, -122.669048));
    }};

    private List<LatLng> ADDITIONAL_POINTS = new ArrayList<LatLng>() {{
        add(new LatLng(45.515369, -122.678489));
        add(new LatLng(45.506346, -122.702007));
        add(new LatLng(45.522585, -122.685699));
    }};

    private MapView mapView;
    private MapboxMap mapboxMap;
    private Polygon polygon;

    private boolean fullAlpha = true;
    private boolean visible = true;
    private boolean allPoints = true;
    private boolean color = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_polygon);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        // configure inital map state
        MapboxMapOptions options = new MapboxMapOptions()
                .attributionTintColor(RED_COLOR)
                .styleUrl(Style.MAPBOX_STREETS)
                .camera(new CameraPosition.Builder()
                        .target(new LatLng(45.520486, -122.673541))
                        .zoom(12)
                        .tilt(40)
                        .build());

        // create map
        mapView = new MapView(this, options);
        mapView.setId(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(this);

        // add to layout
        ((ViewGroup) findViewById(R.id.container)).addView(mapView);
    }

    @Override
    public void onMapReady(MapboxMap mapboxMap) {
        this.mapboxMap = mapboxMap;
        POINTS.addAll(ADDITIONAL_POINTS);
        polygon = mapboxMap.addPolygon(new PolygonOptions()
                .addAll(POINTS)
                .fillColor(BLUE_COLOR));
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
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mapView.onSaveInstanceState(outState);
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

            case R.id.action_id_alpha:
                fullAlpha = !fullAlpha;
                polygon.setAlpha(fullAlpha ? FULL_ALPHA : PARTIAL_ALPHA);
                return true;

            case R.id.action_id_visible:
                visible = !visible;
                polygon.setAlpha(visible ? (fullAlpha ? FULL_ALPHA : PARTIAL_ALPHA) : NO_ALPHA);
                return true;

            case R.id.action_id_points:
                allPoints = !allPoints;
                if (allPoints) {
                    POINTS.addAll(ADDITIONAL_POINTS);
                } else {
                    POINTS.removeAll(ADDITIONAL_POINTS);
                }
                polygon.setPoints(POINTS);
                return true;

            case R.id.action_id_color:
                color = !color;
                polygon.setFillColor(color ? BLUE_COLOR : RED_COLOR);

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_polygon, menu);
        return true;
    }
}
