package com.mapbox.mapboxsdk.testapp.activity.style;

import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;

import java.net.MalformedURLException;
import java.net.URL;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;

/**
 * Use realtime GeoJSON data streams to move a symbol on your map
 * <p>
 * GL-native equivalent of https://www.mapbox.com/mapbox-gl-js/example/live-geojson/
 * </p>
 */
public class RealTimeGeoJsonActivity extends AppCompatActivity implements OnMapReadyCallback {

    private final static String ID_GEOJSON_LAYER = "wanderdrone";
    private final static String ID_GEOJSON_SOURCE = ID_GEOJSON_LAYER;
    private final static String URL_GEOJSON_SOURCE = "https://wanderdrone.appspot.com/";

    private MapView mapView;
    private MapboxMap mapboxMap;

    private Handler handler;
    private Runnable runnable;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_default);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(this);
    }

    @Override
    public void onMapReady(@NonNull final MapboxMap map) {
        mapboxMap = map;

        // add source
        try {
            mapboxMap.addSource(new GeoJsonSource(ID_GEOJSON_SOURCE, new URL(URL_GEOJSON_SOURCE)));
        }catch (MalformedURLException e){
            Log.e(MapboxConstants.TAG, "Invalid URL", e);
        }

        // add layer
        SymbolLayer layer = new SymbolLayer(ID_GEOJSON_LAYER, ID_GEOJSON_SOURCE);
        layer.setProperties(iconImage("rocket-15"));
        mapboxMap.addLayer(layer);

        // loop refresh geojson
        handler = new Handler();
        runnable = new RefreshGeoJsonRunnable(mapboxMap, handler);
        handler.postDelayed(runnable, 2000);
    }

    @Override
    protected void onStart() {
        super.onStart();
        mapView.onStart();
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
    protected void onStop() {
        super.onStop();
        mapView.onStop();
        handler.removeCallbacks(runnable);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mapView.onDestroy();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mapView.onSaveInstanceState(outState);
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

    private static class RefreshGeoJsonRunnable implements Runnable {

        private MapboxMap mapboxMap;
        private Handler handler;

        RefreshGeoJsonRunnable(MapboxMap mapboxMap, Handler handler) {
            this.mapboxMap = mapboxMap;
            this.handler = handler;
        }

        @Override
        public void run() {
            ((GeoJsonSource) mapboxMap.getSource(ID_GEOJSON_SOURCE)).setUrl(URL_GEOJSON_SOURCE);
            handler.postDelayed(this, 2000);
        }
    }
}
