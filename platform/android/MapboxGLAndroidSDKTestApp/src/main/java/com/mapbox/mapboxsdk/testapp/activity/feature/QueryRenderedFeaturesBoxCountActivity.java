package com.mapbox.mapboxsdk.testapp.activity.feature;

import android.graphics.RectF;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import com.google.gson.JsonElement;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.services.commons.geojson.Feature;

import java.util.List;
import java.util.Map;

/**
 * Demo's query rendered features
 */
public class QueryRenderedFeaturesBoxCountActivity extends AppCompatActivity {
    private static final String TAG = QueryRenderedFeaturesBoxCountActivity.class.getSimpleName();

    public MapView mapView;
    private MapboxMap mapboxMap;
    private Marker marker;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_query_features_box);
        setupActionBar();

        final float density = getResources().getDisplayMetrics().density;

        final View selectionBox = findViewById(R.id.selection_box);

        //Initialize map as normal
        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {
            @SuppressWarnings("ConstantConditions")
            @Override
            public void onMapReady(final MapboxMap mapboxMap) {
                QueryRenderedFeaturesBoxCountActivity.this.mapboxMap = mapboxMap;


                selectionBox.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        //Query
                        int top = selectionBox.getTop() - mapView.getTop();
                        int left = selectionBox.getLeft() - mapView.getLeft();
                        RectF box = new RectF(left, top, left + selectionBox.getWidth(), top + selectionBox.getHeight());
                        Log.i(TAG, String.format("Querying box %s", box));
                        List<Feature> features = mapboxMap.queryRenderedFeatures(box);

                        //Show count
                        Toast.makeText(QueryRenderedFeaturesBoxCountActivity.this, String.format("%s features in box", features.size()), Toast.LENGTH_SHORT).show();

                        //Debug output
                        debugOutput(features);
                    }
                });

                //Little taste of home
                mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(52.0907, 5.1214), 16));
            }
        });

    }

    private void debugOutput(List<Feature> features) {
        Log.i(TAG, String.format("Got %s features", features.size()));
        for (Feature feature : features) {
            if (feature != null) {
                Log.i(TAG, String.format("Got feature %s with %s properties and Geometry %s",
                        feature.getId(),
                        feature.getProperties() != null ? feature.getProperties().entrySet().size() : "<null>",
                        feature.getGeometry() != null ? feature.getGeometry().getClass().getSimpleName() : "<null>")
                );
                if (feature.getProperties() != null) {
                    for (Map.Entry<String, JsonElement> entry : feature.getProperties().entrySet()) {
                        Log.i(TAG, String.format("Prop %s - %s", entry.getKey(), entry.getValue()));
                    }
                }
            } else {
                Log.i(TAG, "Got NULL feature %s");
            }
        }
    }

    public MapboxMap getMapboxMap() {
        return mapboxMap;
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
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private void setupActionBar() {
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }
    }

}
