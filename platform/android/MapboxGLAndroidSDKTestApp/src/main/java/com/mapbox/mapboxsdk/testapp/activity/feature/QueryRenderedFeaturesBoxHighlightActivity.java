package com.mapbox.mapboxsdk.testapp.activity.feature;

import android.graphics.Color;
import android.graphics.RectF;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.services.commons.geojson.Feature;
import com.mapbox.services.commons.geojson.FeatureCollection;

import java.util.List;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillColor;

/**
 * Demo's query rendered features
 */
public class QueryRenderedFeaturesBoxHighlightActivity extends AppCompatActivity {
    private static final String TAG = QueryRenderedFeaturesBoxHighlightActivity.class.getSimpleName();

    public MapView mapView;
    private MapboxMap mapboxMap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_query_features_box);
        setupActionBar();

        final View selectionBox = findViewById(R.id.selection_box);

        //Initialize map as normal
        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {
            @SuppressWarnings("ConstantConditions")
            @Override
            public void onMapReady(final MapboxMap mapboxMap) {
                QueryRenderedFeaturesBoxHighlightActivity.this.mapboxMap = mapboxMap;

                //Add a fill layer to display stuff on

                selectionBox.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        //Query
                        int top = selectionBox.getTop() - mapView.getTop();
                        int left = selectionBox.getLeft() - mapView.getLeft();
                        RectF box = new RectF(left, top, left + selectionBox.getWidth(), top + selectionBox.getHeight());
                        Log.i(TAG, String.format("Querying box %s for buildings", box));
                        List<Feature> features = mapboxMap.queryRenderedFeatures(box, "building");

                        //Show count
                        Toast.makeText(
                            QueryRenderedFeaturesBoxHighlightActivity.this,
                            String.format("%s features in box", features.size()),
                            Toast.LENGTH_SHORT).show();

                        //remove layer / source if already added
                        try {
                            mapboxMap.removeSource("highlighted-shapes-source");
                            mapboxMap.removeLayer("highlighted-shapes-layer");
                        } catch (Exception exception) {
                            //that's ok
                        }

                        //Add layer / source
                        mapboxMap.addSource(new GeoJsonSource("highlighted-shapes-source", FeatureCollection.fromFeatures(features)));
                        mapboxMap.addLayer(new FillLayer("highlighted-shapes-layer", "highlighted-shapes-source")
                            .withProperties(fillColor(Color.RED)));
                    }
                });

                //Little taste of home
                mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(52.0907, 5.1214), 16));
            }
        });

    }

    public MapboxMap getMapboxMap() {
        return mapboxMap;
    }

    @Override
    protected void onStart() {
        super.onStart();
        mapView.onStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mapView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mapView.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mapView.onStop();
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
