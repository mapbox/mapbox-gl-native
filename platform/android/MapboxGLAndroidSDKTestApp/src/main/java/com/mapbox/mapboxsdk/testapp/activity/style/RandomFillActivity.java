package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonOptions;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.services.commons.geojson.Feature;
import com.mapbox.services.commons.geojson.FeatureCollection;
import com.mapbox.services.commons.geojson.Polygon;
import com.mapbox.services.commons.models.Position;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

import static com.mapbox.mapboxsdk.style.layers.Filter.all;
import static com.mapbox.mapboxsdk.style.layers.Filter.gt;
import static com.mapbox.mapboxsdk.style.layers.Filter.lt;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillOpacity;

public class RandomFillActivity extends AppCompatActivity {

    private static final String SOURCE_ID = "source_id_color";
    private static final String LAYER_ID_BASE = "layer_id_";
    private static final String LAYER_ID_RED = LAYER_ID_BASE + "red";
    private static final String LAYER_ID_BLUE = LAYER_ID_BASE + "blue";
    private static final String LAYER_ID_YELLOW = LAYER_ID_BASE + "yellow";
    private static final String PROPERTY_ID_KEY = "property_id";
    private static final int RANDOM_FILL_AMOUNT = 210;

    private MapboxMap mapboxMap;
    private MapView mapView;

    private FillLayer redLayer;
    private FillLayer blueLayer;
    private FillLayer yellowLayer;

    private Runnable filterRunnable = new FilterRunnable();
    private Handler handler = new Handler();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_symbollayer);

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
            public void onMapReady(@NonNull final MapboxMap map) {
                mapboxMap = map;
                addFeaturesInSameLayer();
                loopFilter();
            }
        });
    }

    public void addFeaturesInSameLayer() {
        LatLngBounds bounds = new LatLngBounds.Builder().include(new LatLng(70.0d, -30.0d)).include(new LatLng(20.0d, -160.0d)).build();
        List<Feature> polygonFeatures = generateRandomPolygonFeatures(RANDOM_FILL_AMOUNT, bounds, 3, 30);

        GeoJsonSource polygonSource = new GeoJsonSource(SOURCE_ID, FeatureCollection.fromFeatures(polygonFeatures), new GeoJsonOptions());
        mapboxMap.addSource(polygonSource);

        redLayer = new FillLayer(LAYER_ID_RED, SOURCE_ID).withProperties(
                fillColor(Color.RED),
                fillOpacity(0.5f));

        blueLayer = new FillLayer(LAYER_ID_BLUE, SOURCE_ID).withProperties(
                fillColor(Color.BLUE),
                fillOpacity(0.5f));

        yellowLayer = new FillLayer(LAYER_ID_YELLOW, SOURCE_ID).withProperties(
                fillColor(Color.YELLOW),
                fillOpacity(0.5f));

        mapboxMap.addLayer(redLayer);
        mapboxMap.addLayer(blueLayer);
        mapboxMap.addLayer(yellowLayer);
    }

    private List<Feature> generateRandomPolygonFeatures(int quantity, LatLngBounds bounds, int minSides, int maxSides) {
        if (quantity < 0 || bounds == null || minSides < 3 || minSides > maxSides) {
            throw new IllegalArgumentException();
        }
        List<Feature> features = new ArrayList<>(quantity);
        Random rand = new Random();
        for (int i = 0; i < quantity; i++) {
            int numSides = maxSides == minSides ? minSides : rand.nextInt(maxSides - minSides) + minSides;
            double centerLat = rand.nextDouble() * bounds.getLatitudeSpan() + bounds.getLatSouth();
            double centerLng = rand.nextDouble() * bounds.getLongitudeSpan() + bounds.getLonWest();
            List<Position> points = generateRandomSimplePolygon(new LatLng(centerLat, centerLng), numSides, 2.0f, 4.0f);
            Feature feature = Feature.fromGeometry(Polygon.fromCoordinates(Arrays.asList(points)));
            feature.addNumberProperty(PROPERTY_ID_KEY, i);
            features.add(feature);
        }
        return features;
    }

    private List<Position> generateRandomSimplePolygon(LatLng center, int numVertices, float minRadius, float maxRadius) {
        List<Position> points = new ArrayList<>(numVertices + 1);
        Random rand = new Random();
        float angle = 0.0f;
        float angleDelta = 6.28f / numVertices;
        for (int i = 0; i < numVertices; i++) {
            float radius = rand.nextFloat() * (maxRadius - minRadius) + minRadius;
            double lat = center.getLatitude() + radius * Math.sin(angle);
            double lng = center.getLongitude() + radius * Math.cos(angle);
            points.add(Position.fromCoordinates(lng, lat));
            angle += angleDelta;
        }
        points.add(points.get(0));
        return points;
    }

    private void loopFilter() {
        handler.postDelayed(filterRunnable, 3000);
    }

    private class FilterRunnable implements Runnable {

        private int state;

        @Override
        public void run() {
            int lowerBoundry = RANDOM_FILL_AMOUNT / 3;
            int middleBoundry = lowerBoundry * 2;

            if (state == 0) {
                redLayer.setFilter(lt(PROPERTY_ID_KEY, lowerBoundry));
                blueLayer.setFilter(all(gt(PROPERTY_ID_KEY, lowerBoundry), lt(PROPERTY_ID_KEY, middleBoundry)));
                yellowLayer.setFilter(gt(PROPERTY_ID_KEY, middleBoundry));
                state++;
            } else {
                blueLayer.setFilter(lt(PROPERTY_ID_KEY, lowerBoundry));
                yellowLayer.setFilter(all(gt(PROPERTY_ID_KEY, lowerBoundry), lt(PROPERTY_ID_KEY, middleBoundry)));
                redLayer.setFilter(gt(PROPERTY_ID_KEY, middleBoundry));
                state = 0;
            }
            loopFilter();
        }
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
        handler.removeCallbacks(filterRunnable);
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mapView.onSaveInstanceState(outState);
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mapView.onLowMemory();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mapView.onDestroy();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_symbol_layer, menu);
        return true;
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
